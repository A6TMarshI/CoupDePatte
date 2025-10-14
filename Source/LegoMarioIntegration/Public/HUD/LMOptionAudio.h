// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LMOptionsMenu.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "LMOptionAudio.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMOptionAudio : public ULMOptionsMenu
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UCheckBox* CB_TurnOffVolume;

protected:
	virtual void NativeConstruct() override;

	virtual void LoadSettingOptions() override;
	virtual void SaveSettingOptions(ULMOptionsMenu* OptionMenu) override;

private:
	template <class T>
	void AddSlider(UWidget* CurrentWidget, TArray<T*>& OutArray);
	UFUNCTION()
	TArray<USlider*> FindAllSliders();
	UPROPERTY()
	TArray<USlider*> Sliders;
	int FindSliderByName(const TArray<USlider*>& Array, FName Item) const;
};
