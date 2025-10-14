// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LMSaveGameSettings.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMSaveGameSettings : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<FName, float> VolumeMap;
	UPROPERTY(BlueprintReadWrite)
	bool bIsVolumeDisable = false;
};
