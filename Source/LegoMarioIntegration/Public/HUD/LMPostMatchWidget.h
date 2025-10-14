// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LMMenuWidget.h"
#include "Components/TextBlock.h"
#include "LMPostMatchWidget.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMPostMatchWidget : public ULMMenuWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TXT_GameResult;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TXT_SubGameResult;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TXT_MatchSummary;
};
