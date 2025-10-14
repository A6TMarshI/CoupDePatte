// Fill out your copyright notice in the Description page of Project Settings.


#include "Socket/LMTCPConnectionSocket.h"

#include "SocketSubsystem.h"
#include "Socket/LMSocket.h"


bool FLMTCPConnectionSocket::Init()
{
	bStopRequested = false;
	return true;
}

uint32 FLMTCPConnectionSocket::Run()
{
	while (!bStopRequested)
	{
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		bool Pending;

		// handle incoming connections
		if (SocketComponent && SocketComponent->SocketConnection && SocketComponent->SocketConnection->HasPendingConnection(Pending) && Pending)
		{
			FSocket* PendingSocket = SocketComponent->SocketConnection->Accept(*RemoteAddress, TEXT("Received Socket Connection"));
			if (PendingSocket)
			{
				FLMTCPSocketListener* Runnable = new FLMTCPSocketListener(PendingSocket, GameInstance);
				SocketComponent->PendingConnections.Add(FCommunicationData{
					PendingSocket,
					Runnable,
					FRunnableThread::Create(Runnable, TEXT("SocketListenerThread"))
				});
			}
		}
	}
	return 1;
}

void FLMTCPConnectionSocket::Stop()
{
	bStopRequested = true;
}

void FLMTCPConnectionSocket::Initialize(ULMSocket* InSocketComponent)
{
	SocketComponent = InSocketComponent;
	Init();
}

void FLMTCPConnectionSocket::DestroySocket()
{
	Stop();
	SocketComponent = nullptr;
}
