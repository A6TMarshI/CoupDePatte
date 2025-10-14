// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LMLegoReadyWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMLegoReadyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TXT_LegoState;
};
