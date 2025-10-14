// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/LMMarioCharacter.h"
#include "GameFramework/GameMode.h"
#include "LMMarioGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAllControllerConnectedDelegate);

UCLASS()
class LEGOMARIOINTEGRATION_API ALMMarioGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALMMarioGameMode();

	UPROPERTY(BlueprintAssignable)
	FAllControllerConnectedDelegate OnAllControllerConnectedDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

	UPROPERTY(VisibleAnywhere, Category="MatchSettings", BlueprintReadWrite, meta=(AllowPrivateAccess))
	FString Username;
	
private:
	UPROPERTY(EditAnywhere, Category="CharacterClass")
	TSubclassOf<ALMMarioCharacter> MarioCharacterClass;
	UPROPERTY(EditAnywhere, Category="CharacterClass")
	TSubclassOf<ALMMarioController> MarioControllerClass;

	void SpawnSecondLegoPlayer();

public:
	FORCEINLINE FString GetUsername() const { return Username; }
};
