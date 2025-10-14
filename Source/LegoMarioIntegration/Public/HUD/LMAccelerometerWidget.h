// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "LMAccelerometerWidget.generated.h"


/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMAccelerometerWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UVerticalBox* VB_Wrapper;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_Info;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_PosX;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_PosY;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_PosZ;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_Pant;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_Ground;
};
