// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "GameInstances/LMGameInstance.h"
#include "PlayerControllers/LMMarioController.h"


/**
 * 
 */
class LEGOMARIOINTEGRATION_API FLMTCPSocketListener : public FRunnable
{
public:
	friend ULMSocket;

	FLMTCPSocketListener(FSocket* InConnectionSocket, ULMGameInstance* InGameInstance) :
		ConnectionSocket(InConnectionSocket),
		GameInstance(InGameInstance)
	{
	}

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	void Pause(bool bPaused);

private:
	FSocket* ConnectionSocket = nullptr;
	ALMMarioController* MarioController = nullptr;
	ULMGameInstance* GameInstance = nullptr;
	bool bSendToPauseMenu = false;
	bool bStopRequested = false;
	bool bPauseRequested = false;
	int LegoOutfit = 0;
	FString LegoGroundColor = FString();
	FString LegoName = FString();

public:
	void SetMarioController(ALMMarioController* InMarioController) { MarioController = InMarioController; }
	FORCEINLINE FString GetGroundColor() const { return LegoGroundColor; }
	FORCEINLINE FString GetLegoName() const { return LegoName; }
};
