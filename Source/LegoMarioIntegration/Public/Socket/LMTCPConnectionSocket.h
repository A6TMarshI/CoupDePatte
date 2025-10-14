// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameInstances/LMGameInstance.h"


class LEGOMARIOINTEGRATION_API FLMTCPConnectionSocket : public FRunnable
{
public:
	friend class ULMSocket;

	FLMTCPConnectionSocket(ULMSocket* InSocketComponent, ULMGameInstance* InGameInstance) :
		SocketComponent(InSocketComponent),
		GameInstance(InGameInstance)
	{
	}


	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	void Initialize(ULMSocket* InSocketComponent);
	void DestroySocket();

private:
	bool bStopRequested = false;
	ULMSocket* SocketComponent = nullptr;
	ULMGameInstance* GameInstance = nullptr;
};
