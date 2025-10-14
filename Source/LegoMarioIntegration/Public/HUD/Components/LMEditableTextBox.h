// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "LMEditableTextBox.generated.h"

class ULMSlider;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustomTextCommitted, ULMSlider*, Slider, float, Value);

UCLASS()
class LEGOMARIOINTEGRATION_API ULMEditableTextBox : public UEditableTextBox
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ULMSlider* LMSlider;

	UPROPERTY(BlueprintAssignable)
	FOnCustomTextCommitted OnCustomTextCommittedDelegate;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void CustomHandleTextCommitted(const FText& InText, ETextCommit::Type Commit);
};
