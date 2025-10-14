// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LMOptionVideo.h"

#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"


void ULMOptionVideo::NativeConstruct()
{
	Super::NativeConstruct();
	
	PopulateSettingOptionsMap(HB_Resolution);
	PopulateSettingOptionsMap(HB_AntiAliasing);
	PopulateSettingOptionsMap(HB_FrameLimit);
	PopulateSettingOptionsMap(HB_PostProcess);
	PopulateSettingOptionsMap(HB_ShadowQuality);
	PopulateSettingOptionsMap(HB_ViewDistance);
	PopulateSettingOptionsMap(HB_WindowMode);

}

void ULMOptionVideo::LoadSettingOptions()
{
	Super::LoadSettingOptions();

	if (SettingOptions.Num() <= 0)return;

	/**
	 * Window Mode
	 */
	UserSettings->GetFullscreenMode() == EWindowMode::Fullscreen
		? SetSelectionColor(HB_WindowMode, SettingOptions[HB_WindowMode].OptionButtons[0])
		: SetSelectionColor(HB_WindowMode, SettingOptions[HB_WindowMode].OptionButtons[1]);


	/**
	 * Resolution
	 */
	FIntPoint ActualResolution = UserSettings->GetScreenResolution();
	for (auto Button : SettingOptions[HB_Resolution].OptionButtons)
	{
		if (auto Text = Cast<UTextBlock>(Cast<UPanelWidget>(Button)->GetChildAt(0)))
		{
			if (Text->GetText().ToString() == FString::Printf(TEXT("%dx%d"), ActualResolution.X, ActualResolution.Y))
			{
				SetSelectionColor(HB_Resolution, Button);
				SetScreenResolution(Button);
			}
		}
	}
	/**
	 * Frame Limit
	 */
	switch (FMath::TruncToInt(UserSettings->GetFrameRateLimit()))
	{
	case 60:
		SetSelectionColor(HB_FrameLimit, SettingOptions[HB_FrameLimit].OptionButtons[0]);
		break;
	case 144:
		SetSelectionColor(HB_FrameLimit, SettingOptions[HB_FrameLimit].OptionButtons[1]);
		break;
	case 0:
		SetSelectionColor(HB_FrameLimit, SettingOptions[HB_FrameLimit].OptionButtons[2]);
		break;
	default:
		break;
	}

	SetSelectionColor(HB_ViewDistance, SettingOptions[HB_ViewDistance].OptionButtons[UserSettings->GetViewDistanceQuality()]);
	SetSelectionColor(HB_PostProcess, SettingOptions[HB_PostProcess].OptionButtons[UserSettings->GetPostProcessingQuality()]);
	SetSelectionColor(HB_AntiAliasing, SettingOptions[HB_AntiAliasing].OptionButtons[UserSettings->GetAntiAliasingQuality()]);
	SetSelectionColor(HB_ShadowQuality, SettingOptions[HB_ShadowQuality].OptionButtons[UserSettings->GetShadowQuality()]);
}

void ULMOptionVideo::SetScreenResolution(UButton* ResolutionButton)
{
	if (ResolutionButton == nullptr)return;
	for (auto Element : ResolutionButton->GetAllChildren())
	{
		if (auto Text = Cast<UTextBlock>(Element))
		{
			TArray<FString> Resolutions;
			if (Text->GetText().ToString().ParseIntoArray(Resolutions,TEXT("x")) == 2)
			{
				FIntPoint Resolution;
				Resolution.X = FCString::Atoi(*Resolutions[0]);
				Resolution.Y = FCString::Atoi(*Resolutions[1]);
				UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
				UserSettings->SetScreenResolution(Resolution);
				UserSettings->ApplyResolutionSettings(false);
			}
		}
	}
}

void ULMOptionVideo::SetFPSCap(float FPSCap)
{
	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->SetFrameRateLimit(FPSCap);
}

void ULMOptionVideo::SetViewDistance(int ViewDistance)
{
	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->SetViewDistanceQuality(ViewDistance);
}

void ULMOptionVideo::SetPostProcessQuality(int PostProcessQuality)
{
	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->SetPostProcessingQuality(PostProcessQuality);
}

void ULMOptionVideo::SetAntiAliasingQuality(int AntiAliasingQuality)
{
	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->SetAntiAliasingQuality(AntiAliasingQuality);
}

void ULMOptionVideo::SetShadowQuality(int ShadowQuality)
{
	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->SetShadowQuality(ShadowQuality);
}

void ULMOptionVideo::SetWindowMode(EWindowMode::Type WindowMode)
{
	UserSettings = UserSettings == nullptr ? UGameUserSettings::GetGameUserSettings() : UserSettings;
	UserSettings->SetFullscreenMode(WindowMode);
	UserSettings->ApplyResolutionSettings(false);
}
