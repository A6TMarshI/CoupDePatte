// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LMOptionAudio.h"

#include "Blueprint/WidgetTree.h"
#include "HelperLibrary/LMSaveGameSettings.h"
#include "Kismet/GameplayStatics.h"

void ULMOptionAudio::NativeConstruct()
{
	Super::NativeConstruct();
	Sliders = FindAllSliders();


	LoadSettingOptions();
}

void ULMOptionAudio::LoadSettingOptions()
{
	Super::LoadSettingOptions();

	if (const auto Settings = Cast<ULMSaveGameSettings>(UGameplayStatics::LoadGameFromSlot(FString("Audio"), 0)))
	{
		if (Settings->VolumeMap.IsEmpty())
		{
			SaveSettingOptions(this);
			LoadSettingOptions();
		}
		else
		{
			CB_TurnOffVolume->SetIsChecked(Settings->bIsVolumeDisable);
			for (const auto VolumePair : Settings->VolumeMap)
			{
				const int Index = FindSliderByName(Sliders, VolumePair.Key);
				if (Index != INDEX_NONE)
				{
					Sliders[Index]->SetValue(VolumePair.Value);
					Sliders[Index]->OnValueChanged.Broadcast(VolumePair.Value);
				}
			}
		}
	}
}

void ULMOptionAudio::SaveSettingOptions(ULMOptionsMenu* OptionMenu)
{
	Super::SaveSettingOptions(OptionMenu);

	const auto SavedSettings = Cast<ULMSaveGameSettings>(UGameplayStatics::CreateSaveGameObject(ULMSaveGameSettings::StaticClass()));
	SavedSettings->VolumeMap.Empty();
	for (const auto Slider : Sliders)
	{
		SavedSettings->VolumeMap.Add(FName(Slider->GetName()), Slider->GetValue());
	}
	SavedSettings->bIsVolumeDisable = CB_TurnOffVolume->IsChecked();
	UGameplayStatics::SaveGameToSlot(SavedSettings, FString("Audio"), 0);
}

TArray<USlider*> ULMOptionAudio::FindAllSliders()
{
	TArray<UWidget*> Widgets;
	this->WidgetTree.Get()->GetAllWidgets(Widgets);
	for (const auto Widget : Widgets)
	{
		AddSlider<USlider>(Widget, Sliders);
	}

	return Sliders;
}

template <class T>
void ULMOptionAudio::AddSlider(UWidget* CurrentWidget, TArray<T*>& OutArray)
{
	if (!CurrentWidget)return;

	if (T* Component = Cast<T>(CurrentWidget))
	{
		OutArray.Add(Component);
	}
}

int ULMOptionAudio::FindSliderByName(const TArray<USlider*>& Array, FName Item) const
{
	for (int i = 0; i < Array.Num(); i++)
	{
		if (Array[i] && FName(Array[i]->GetName()) == Item)
		{
			return i;
		}
	}
	return INDEX_NONE;
}
