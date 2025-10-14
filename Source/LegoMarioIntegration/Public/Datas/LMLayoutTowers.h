// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "LMLayoutTowers.generated.h"

UENUM(BlueprintType)
enum class ETowerHeight : uint8
{
	ETH_0 UMETA(DisplayName = "À plat"),
	ETH_10 UMETA(DisplayName = "Hauteur variable"),
	ETH_20 UMETA(DisplayName = "Hauteur variable"),
	ETH_Empty UMETA(DisplayName = "EMPTY")
};

UENUM(BlueprintType)
enum class ETowerAngle : uint8
{
	ETA_0 UMETA(DisplayName = "0°"),
	ETA_45 UMETA(DisplayName = "45°"),
	ETA_90 UMETA(DisplayName = "90°"),
	ETA_Empty UMETA(DisplayName = "EMPTY")
};

UENUM(BlueprintType)
enum class ETowerMovement : uint8
{
	ETM_Flat UMETA(DisplayName = "Plat"),
	ETM_Extension UMETA(DisplayName = "Extension"),
	ETM_Supination UMETA(DisplayName = "Supination"),
	ETM_Both UMETA(DisplayName = "Les Deux"),
	ETM_Empty UMETA(DisplayName = "EMPTY")
};

USTRUCT(BlueprintType)
struct FLayoutActivatedTowers : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="LayoutProperties", BlueprintReadOnly, meta=(Categories="PixelArt"))
	FGameplayTag LayoutTag = TAG_NONE;
	UPROPERTY(EditAnywhere, Category="LayoutProperties", BlueprintReadOnly)
	TArray<FString> LayoutInfos = TArray<FString>();
};

UCLASS(Blueprintable)
class LEGOMARIOINTEGRATION_API ULMLayoutTowers : public UDataTable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly)
	TArray<FLayoutActivatedTowers> LayoutInfos;

	static TArray<FString> FindLayoutInfo(const FGameplayTag& LayoutTag, const TArray<FLayoutActivatedTowers*>& Array, const int Offset);

	static TArray<FGameplayTag> GetLevelRelatedPixelTags(const TArray<FLayoutActivatedTowers*>& Array, const int LevelIndex);
	UFUNCTION(BlueprintCallable)
	static TArray<FGameplayTag> K2_GetLevelRelatedPixelTags(UPARAM(ref) const TArray<FLayoutActivatedTowers>& Array, const int LevelIndex);
};
