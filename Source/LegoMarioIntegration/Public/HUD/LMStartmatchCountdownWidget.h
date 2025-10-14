// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "UObject/Object.h"
#include "LMStartmatchCountdownWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNameCommittedDelegate, const FString&, Name);

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMStartmatchCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_Countdown;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_SubLevel;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UBorder* B_Settings;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_Countdown;

	UPROPERTY(BlueprintCallable)
	FNameCommittedDelegate OnNameCommittedDelegate;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ConfirmStartGame();
	UFUNCTION(BlueprintCallable)
	virtual UWorld* GetWorld() const override;
};
