// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PALoadMatLab.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FMatLabInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="MatLabProperties", BlueprintReadOnly)
	int X = 0;
	UPROPERTY(EditAnywhere, Category="MatLabProperties", BlueprintReadOnly)
	int Y = 0;
	UPROPERTY(EditAnywhere, Category="MatLabProperties", BlueprintReadOnly)
	float R = 0;
	UPROPERTY(EditAnywhere, Category="MatLabProperties", BlueprintReadOnly)
	float G = 0;
	UPROPERTY(EditAnywhere, Category="MatLabProperties", BlueprintReadOnly)
	float B = 0;
};

UCLASS()
class PIXELART_API UPALoadMatLab : public UDataTable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="MatLabProperties")
	TArray<FMatLabInfo> MatLabInfos;
};
