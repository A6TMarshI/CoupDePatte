// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "LMTCPConnectionSocket.h"
#include "LMTCPSocketListener.h"
#include "LMSocket.generated.h"

USTRUCT()
struct FCommunicationData
{
	GENERATED_BODY()

	FSocket* PendingConnectionSocket;
	FLMTCPSocketListener* LegoListenerThread;
	FRunnableThread* LegoListenerRunnable;
};

class FLMTCPSocketListener;
/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LEGOMARIOINTEGRATION_API ULMSocket : public UActorComponent
{
	GENERATED_BODY()

public:
	friend ULMGameInstance;

	void InitializeSocket(ULMGameInstance* InGameInstance);
	void DestroySocketComponents(const bool& bCompletly = false);
	void BindToLego(ALMMarioController* Controller);
	void PauseThread(bool bPause);

	void DestroySocketConnection();
	void DestroySocketListener();
	void DestroySocketListenerByName(const FString& Name);

	UPROPERTY()
	ULMGameInstance* GameInstance;
	FSocket* SocketConnection;
	FLMTCPConnectionSocket* SocketConnectionThread;
	FRunnableThread* SocketConnectionRunnable;


	TArray<FCommunicationData> PendingConnections;

	TArray<FCommunicationData>::SizeType GetCommunicationDataIndexByListenerThreadName(const FString& Name);

protected:
	virtual void BeginPlay() override;

	bool StartTCPReceiver(const FString& SocketName, const FString& IP, const int32 Port);
	FSocket* CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize = 2 * 1024 * 1024);

	bool FormatIP4ToNumber(const FString& IP, uint8 (&Out)[4]);

private:
	const int PortEntry = 7777;
	const FString ExecutableInternalPath = FString("Externals/SocketExecutables/mario_sample.exe");
	const int ExecutableCount = 2;
	TArray<FProcHandle> ExecutableHandles;
};
