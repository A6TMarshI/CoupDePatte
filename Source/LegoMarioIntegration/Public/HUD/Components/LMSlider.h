// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "LMSlider.generated.h"

/**
 * 
 */

class ULMEditableTextBox;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValueChanged, ULMEditableTextBox*, EditableTextBox, float, Value);

UCLASS()
class LEGOMARIOINTEGRATION_API ULMSlider : public USlider
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ULMEditableTextBox* LMEditableTextBox;

	UPROPERTY(BlueprintAssignable)
	FOnValueChanged OnValueChangedDelegate;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION(BlueprintCallable)
	void HandleCustomValueChanged(float InValue);
};
