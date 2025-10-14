// Fill out your copyright notice in the Description page of Project Settings.


#include "Socket/LMSocket.h"

#include "GenericPlatform/GenericPlatformFile.h"
#include "Windows/WindowsHWrapper.h"
#include "TimerManager.h"
#include "Common/TcpSocketBuilder.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "PlayerControllers/LMMarioController.h"
#include "Socket/LMTCPSocketListener.h"

class FLMTCPSocketListener;

TArray<FCommunicationData>::SizeType ULMSocket::GetCommunicationDataIndexByListenerThreadName(const FString& Name)
{
	const TArray<FCommunicationData>::ElementType* RESTRICT Start = PendingConnections.GetData();
	for (const TArray<FCommunicationData>::ElementType * RESTRICT Data = Start, *RESTRICT DataEnd = Data + PendingConnections.Num(); Data != DataEnd; ++Data)
	{
		if (Data->LegoListenerThread->LegoName.Contains(Name) && !Name.IsEmpty())
		{
			return static_cast<TArray<FCommunicationData>::SizeType>(Data - Start);
		}
	}
	return INDEX_NONE;
}

void ULMSocket::BeginPlay()
{
	UActorComponent::BeginPlay();
}

void ULMSocket::DestroySocketConnection()
{
	if (SocketConnectionThread)SocketConnectionThread->DestroySocket();
	SocketConnection->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SocketConnection);
	delete SocketConnectionRunnable;
	delete SocketConnectionThread;
	SocketConnectionThread = nullptr;
	SocketConnectionRunnable = nullptr;
	SocketConnection = nullptr;
}

void ULMSocket::DestroySocketListener()
{
	for (auto PendingConnection : PendingConnections)
	{
		if (PendingConnection.LegoListenerThread)PendingConnection.LegoListenerThread->Stop();
		PendingConnection.PendingConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(PendingConnection.PendingConnectionSocket);
		PendingConnection.LegoListenerThread->Stop();
		PendingConnection.LegoListenerRunnable->Kill();
		delete PendingConnection.LegoListenerThread;
		delete PendingConnection.LegoListenerRunnable;
		PendingConnection.LegoListenerThread = nullptr;
		PendingConnection.LegoListenerRunnable = nullptr;
		PendingConnection.PendingConnectionSocket = nullptr;
	}
}

void ULMSocket::DestroySocketListenerByName(const FString& Name)
{
	const int Index = GetCommunicationDataIndexByListenerThreadName(Name);
	if (Index != INDEX_NONE)
	{
		if (PendingConnections[Index].LegoListenerThread)PendingConnections[Index].LegoListenerThread->Stop();
		PendingConnections[Index].PendingConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(PendingConnections[Index].PendingConnectionSocket);
		PendingConnections[Index].LegoListenerThread->Stop();
		PendingConnections[Index].LegoListenerRunnable->Kill();
		delete PendingConnections[Index].LegoListenerThread;
		delete PendingConnections[Index].LegoListenerRunnable;
		PendingConnections[Index].LegoListenerThread = nullptr;
		PendingConnections[Index].LegoListenerRunnable = nullptr;
		PendingConnections[Index].PendingConnectionSocket = nullptr;
		PendingConnections.RemoveAt(Index);
	}
}

void ULMSocket::DestroySocketComponents(const bool& bCompletly)
{
	DestroySocketListener();
	DestroySocketConnection();
	for (auto ExecutableHandle : ExecutableHandles)
	{
		if (ExecutableHandle.IsValid())
		{
			FPlatformProcess::TerminateProc(ExecutableHandle, true);
			ExecutableHandle.Reset();
		}
	}
	ExecutableHandles.Empty();
}

void ULMSocket::BindToLego(ALMMarioController* Controller)
{
	const int Index = GetCommunicationDataIndexByListenerThreadName(Controller->NetPlayerIndex == 0 ? "Mario" : "Luigi");
	if (Index != INDEX_NONE)
	{
		PendingConnections[Index].LegoListenerThread->MarioController = Controller;
		Controller->SetMarioOutfit(PendingConnections[Index].LegoListenerThread->LegoOutfit);
		Controller->VerifyGroundColor(PendingConnections[Index].LegoListenerThread->LegoGroundColor);
	}
}

void ULMSocket::PauseThread(bool bPause)
{
	for (auto PendingConnection : PendingConnections)
	{
		if (PendingConnection.LegoListenerThread)PendingConnection.LegoListenerThread->Pause(bPause);
	}
}

void ULMSocket::InitializeSocket(ULMGameInstance* InGameInstance)
{
	GameInstance = InGameInstance;
	bool bCanBindAll = true;
	const FString LocalAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll)->
	                                                                              ToString(false);
	if (!StartTCPReceiver("SocketListener", LocalAddress, PortEntry))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create the Socket Listener"));
		return;
	}
	FString ExecutablePath = FPaths::ProjectContentDir();
	ExecutablePath.Append(ExecutableInternalPath);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ExecutablePath);
	for (int i = 0; i < ExecutableCount; i++)
	{
		ExecutableHandles.Add(FPlatformProcess::CreateProc(*ExecutablePath, *FString(), true, true, false, nullptr, 10, nullptr,
		                                                   nullptr));
	}
}

bool ULMSocket::StartTCPReceiver(const FString& SocketName, const FString& IP, const int32 Port)
{
	SocketConnection = CreateTCPConnectionListener(SocketName, IP, Port);

	if (!SocketConnection)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Listen socket could not be created! INFO: %s %d"), *FString(__FUNCTION__),
		       *IP, Port);
		return false;
	}

	//Start the Listener!
	if (!SocketConnectionThread) SocketConnectionThread = new FLMTCPConnectionSocket(this, GameInstance);
	else SocketConnectionThread->Initialize(this);
	SocketConnectionRunnable = FRunnableThread::Create(SocketConnectionThread, TEXT("SocketConnectionThread"));
	return true;
}

bool ULMSocket::FormatIP4ToNumber(const FString& IP, uint8 (&Out)[4])
{
	//IP Formatting
	FString FormattedIP = IP.Replace(TEXT(" "), TEXT(""));

	//String Parts
	TArray<FString> Sections;
	FormattedIP.ParseIntoArray(Sections, TEXT("."), true);
	if (Sections.Num() != 4)
		return false;

	for (int32 i = 0; i < 4; ++i)
	{
		Out[i] = FCString::Atoi(*Sections[i]);
	}

	return true;
}

FSocket* ULMSocket::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port,
                                                const int32 ReceiveBufferSize)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(IP, IP4Nums))
	{
		UE_LOG(LogTemp, Warning, TEXT("IP ill formed."));
		return nullptr;
	}

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), Port);
	FSocket* ListenSocket = FTcpSocketBuilder(*SocketName)
	                        .AsNonBlocking()
	                        .AsReusable()
	                        .BoundToPort(Port)
	                        .WithReceiveBufferSize(ReceiveBufferSize);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);
	ListenSocket->Listen(8);

	return ListenSocket;
}
