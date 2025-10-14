// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/LMTopDownGameMode.h"
#include "Gameplay/LMLegoTower.h"
#include "Gameplay/LMMoleTower.h"
#include "HelperLibrary/LMSaveLevelSettings.h"
#include "LMPlayerState.generated.h"

/**
 * 
 */

UCLASS()
class LEGOMARIOINTEGRATION_API ALMPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, FPerLegoHit> HitMoleTelemetryInfo;

protected:
	virtual void BeginPlay() override;

private:
	int Health = 0;

public:
	void RemoveHealth(const int Removed = 1);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetHealth() const { return Health; }

	FORCEINLINE void SetHealth(const int MatchHealth) { Health = MatchHealth; }
};
