// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/LMLegoTower.h"
#include "Gameplay/LMMoleTower.h"
#include "PlayerControllers/LMMarioController.h"
#include "LMWhackAMoleController.generated.h"

/**
 * 
 */

enum class EHandTrained : uint8;
class ALMWhackAMoleGameMode;

UCLASS()
class LEGOMARIOINTEGRATION_API ALMWhackAMoleController : public ALMMarioController
{
	GENERATED_BODY()

public:
	virtual void SetMarioDirection(FVector DirectionVector) override;
	virtual void OnMatchStateSet(const FName MatchState) override;

	UFUNCTION(BlueprintCallable)
	void SetCameraByTrainedHand();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	/// Within Whack-a-Mole Context, Ground event it either related to Reloading Mario or Luigi Capacity (Red or Green)
	/// Either Hit A Tower (any remaining colors)
	/// @param GroundData <see cref="TriggerMarioGroundOrTileEvent\">
	/// @param GroundColor <see cref="TriggerMarioGroundOrTileEvent\">
	virtual void TriggerMarioGroundOrTileEvent(const EGroudAndTileColor GroundData, FLinearColor& GroundColor) override;
	/// This is responsible for releading the current players turn lego if ground event match players color. Then gamemode get notified as gameplay is locked until the last players turn has been placed back on its corresponding tower
	/// This ensure user does not enter huge loose streak from not being able to place handle lego back in place fast enough
	/// @param GroundColor Only seeking Red or Green Color for either Mario or Luigi
	/// @param LegoTower The current player's turn, either Mario or Luigi
	/// @param GameMode Whack-a-Mole Gamemode.
	void ReloadLegoAttack(const FLinearColor& GroundColor, ALMLegoTower* LegoTower, const ALMWhackAMoleGameMode* GameMode);

private:
	UPROPERTY(EditAnywhere, Category="SoundProperties")
	USoundBase* ReloadSound;
	UPROPERTY(EditAnywhere, Category="SoundProperties")
	USoundBase* FireBallSound;
};
