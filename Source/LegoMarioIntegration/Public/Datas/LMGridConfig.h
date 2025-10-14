// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "LMGridConfig.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FGridInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly, meta=(Categories="Data"))
	FGameplayTag TileShape = TAG_NONE;
	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly)
	FVector MeshSize = FVector::Zero();
	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly)
	TSubclassOf<AActor> Actor = nullptr;
	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly)
	UStaticMesh* FlatStaticMesh = nullptr;
	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly)
	UMaterialInstance* FlatMaterial = nullptr;
};

UCLASS(Blueprintable)
class LEGOMARIOINTEGRATION_API ULMGridConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="GridProperties")
	TArray<FGridInfo> GridInfos;

	const FGridInfo* FindGridInfo(const FGameplayTag& TileShapeTag) const;
};
