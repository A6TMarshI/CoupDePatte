// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LMOptionsMenu.h"

#include "Components/Button.h"
#include "GameFramework/GameUserSettings.h"
#include "GameInstances/LMGameInstance.h"
#include "Socket/LMSocket.h"

void ULMOptionsMenu::VisibilityChanged(ESlateVisibility InVisibility)
{
	if (auto* GameInstance = Cast<ULMGameInstance>(GetGameInstance()))
	{
		switch (InVisibility)
		{
		case ESlateVisibility::Visible:
			GameInstance->PauseThread(true);
			break;
		case ESlateVisibility::Hidden:
			GameInstance->PauseThread(false);
			break;
		default:
			break;
		}
	}
}

void ULMOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->LoadSettings(true);

	OnVisibilityChanged.AddDynamic(this, &ThisClass::VisibilityChanged);
}


void ULMOptionsMenu::PopulateSettingOptionsMap(UHorizontalBox* HorizontalBox)
{
	TArray<UButton*> Buttons;
	for (const auto Element : HorizontalBox->GetAllChildren())
	{
		if (auto ElementChildButton = Cast<UButton>(Element))
		{
			Buttons.Add(ElementChildButton);
		}
		else if (const auto ElementChildBorder = Cast<UPanelWidget>(Element))
		{
			for (const auto ElementChild : ElementChildBorder->GetAllChildren())
			{
				if (auto Button = Cast<UButton>(ElementChild))
				{
					Buttons.Add(Button);
				}
			}
		}
	}
	SettingOptions.Emplace(HorizontalBox, FLastOptionSelected{Buttons, nullptr});
}

void ULMOptionsMenu::SetSelectionColor(UHorizontalBox* SelectedOptionHorizontalBox, UButton* SelectedOptionButton)
{
	if (!SettingOptions.Contains(SelectedOptionHorizontalBox))
	{
		TArray<UButton*> Buttons;
		for (const auto Element : SelectedOptionHorizontalBox->GetAllChildren())
		{
			if (auto ElementChildButton = Cast<UButton>(Element))
			{
				Buttons.Add(ElementChildButton);
			}
			else if (const auto ElementChildBorder = Cast<UPanelWidget>(Element))
			{
				for (const auto ElementChild : ElementChildBorder->GetAllChildren())
				{
					if (auto Button = Cast<UButton>(ElementChild))
					{
						Buttons.Add(Button);
					}
				}
			}
		}
		SettingOptions.Emplace(SelectedOptionHorizontalBox, FLastOptionSelected{Buttons, SelectedOptionButton});
		SelectedOptionButton->SetBackgroundColor(SelectedColor);
	}
	else
	{
		if (SettingOptions[SelectedOptionHorizontalBox].LastOptionSelected != nullptr)
		{
			SettingOptions[SelectedOptionHorizontalBox].LastOptionSelected->SetBackgroundColor(UnselectedColor);
		}
		SelectedOptionButton->SetBackgroundColor(SelectedColor);
		SettingOptions[SelectedOptionHorizontalBox].LastOptionSelected = SelectedOptionButton;
	}
}

void ULMOptionsMenu::SaveSettingOptions(ULMOptionsMenu* OptionMenu)
{
	OptionMenu->UserSettings->ApplySettings(false);
	UserSettings = OptionMenu->UserSettings;
}

void ULMOptionsMenu::SetWidgetVisibilityOnClick(UUserWidget* InWidget)
{
	InWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ULMOptionsMenu::SetSettingsOptionButton(UButton* ActiveButton, int SwitcherIndex)
{
	Options_Switcher->SetActiveWidgetIndex(SwitcherIndex);
	Options_Switcher->GetActiveWidget()->SetVisibility(ESlateVisibility::Visible);
	SetSelectionColor(HB_SettingsOption, ActiveButton);
}

void ULMOptionsMenu::ResetToDefault()
{
	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->LoadSettings(true);
}

void ULMOptionsMenu::LoadSettingOptions()
{
	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->LoadSettings(true);
}
