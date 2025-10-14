// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerControllers/LMMarioController.h"
#include "LMGameInstance.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConnectionEstablishedDelegate, FString, Name, bool, bConnected);

UCLASS()
class LEGOMARIOINTEGRATION_API ULMGameInstance : public UGameInstance
{
	GENERATED_BODY()

	ULMGameInstance();
/*
 * Current laydown from the architecture of socket communication is that we don't have a single connection listening socket. There is a new one created every time we want to create connection with a LEGO, which means it destroyed every time there is a disconnection as well.
 * This could be changed in order to have a single connection listening socket. More over the external app gets shutdown everytime a disconnection occurs which could be better handle from separating connection listener and lego socket listener
 * The current LEGO socket listener get assign from having a connection which is fine, but depending on what port we expect to listen from which is not optimal.
 * FOr better communication, the connection listener should be unique listening from a single given port and assigning LegoSocketListeners on having a connection request. Having a connection mapping could handle reliability in disconnecting/reconnecting legos
 */
public:
	UPROPERTY()
	FConnectionEstablishedDelegate OnConnectionEstablishedDelegate;
	TArray<FVector> CalibrationOffsets;

	void BindMarioSocket(ALMMarioController* InController);
	void BindLuigiSocket(ALMMarioController* InController);
	bool IsReadyForBinding();
	void PauseThread(bool bPause);

protected:
	virtual void OnStart() override;
	virtual void Shutdown() override;

private:
	UPROPERTY(VisibleAnywhere)
	class ULMSocket* SocketComponent;
	bool bReadyForBinding = false;

	void CreateSocketConnection();
	UFUNCTION()
	void HandleLegoConnection(FString Name, bool bConnected);
};
