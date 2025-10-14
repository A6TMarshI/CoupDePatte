// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LMOptionsMenu.h"
#include "Components/EditableTextBox.h"
#include "Components/LMEditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "LMLevelSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMLevelSelectionWidget : public ULMOptionsMenu
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LevelsProperties")
	float NextSpawnTime;

protected:
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;

	bool CreateHorizontalButtonStructure(UHorizontalBox* HBHeight, const FString& DecText, const FString& IncText, const FString& Key);
	UFUNCTION()
	void HandleSliderValueChanged(ULMEditableTextBox* EditableTextBox, float Value);
	UFUNCTION()
	void HandleETextComitted(ULMSlider* Slider, float Value);

private:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* SB_Levels;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="LevelsProperties", meta = (AllowPrivateAccess = "true"))
	TArray<ULMSlider*> Sliders;

	const int LevelPerRow = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LevelProperties", meta = (AllowPrivateAccess = "true"))
	int LevelCount = 62;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LevelProperties", meta = (AllowPrivateAccess = "true"))
	ESlateVisibility OptionVisibility = ESlateVisibility::Hidden;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LevelProperties", meta = (AllowPrivateAccess = "true"))
	float MaxSliderValue = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LevelProperties", meta = (AllowPrivateAccess = "true"))
	float MinSliderValue = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LevelProperties", meta = (AllowPrivateAccess = "true"))
	float DefaultValue = 0.1f;
};
