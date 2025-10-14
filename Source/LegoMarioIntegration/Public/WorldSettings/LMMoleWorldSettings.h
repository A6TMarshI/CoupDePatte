// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/WorldSettings.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "LMMoleWorldSettings.generated.h"

class APAWorldPixelArt;
/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ALMMoleWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleAudioFinished();

	UPROPERTY(EditAnywhere, Category="LevelSettings", meta=(Categories="PixelArt"))
	FGameplayTag PixelArtTag = TAG_NONE;

	UPROPERTY()
	UAudioComponent* AudioComponent = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FGameplayTag GetLevelPixelTag() const { return PixelArtTag; }
};
