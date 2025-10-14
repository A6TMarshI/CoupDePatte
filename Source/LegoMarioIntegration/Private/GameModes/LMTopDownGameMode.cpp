// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LMTopDownGameMode.h"
#include "Engine/PostProcessVolume.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

void ALMTopDownGameMode::AddAllowedColors()
{
	Colors.Empty();

	Colors.Add(PURPLE_COLOR);
	Colors.Add(WHITE_COLOR);
	Colors.Add(BLUE_COLOR);
	Colors.Add(BROWN_COLOR);
	Colors.Add(CYAN_COLOR);
	Colors.Add(YELLOW_COLOR);
}

void ALMTopDownGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (Colors.IsEmpty()) AddAllowedColors();

	InitializePostProcess();
}

void ALMTopDownGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
}

void ALMTopDownGameMode::InitializePostProcess()
{
	TSubclassOf<AActor> PostProcessClass = APostProcessVolume::StaticClass();


	if (auto PostProcess = Cast<
		APostProcessVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), PostProcessClass)))
	{
		PostProcessVolume = PostProcess;
		PostProcessMaterialInstance = UMaterialInstanceDynamic::Create(PostProcessMaterialInstanceClass, nullptr);
		PostProcessVolume->AddOrUpdateBlendable(PostProcessMaterialInstance);
	}
}

bool ALMTopDownGameMode::IsMatchInProgress() const
{
	if (GetMatchState() == MatchState::PlayGame || Super::IsMatchInProgress())
	{
		return true;
	}
	return false;
}

FLinearColor ALMTopDownGameMode::GetNextColor(const bool bClearAllowedColors)
{
	if (bClearAllowedColors || Colors.IsEmpty()) AddAllowedColors();
	const FLinearColor Color = Colors.Pop();
	return Color;
}

void ALMTopDownGameMode::SetVignetteColor(FLinearColor VignetteColor)
{
	if (PostProcessMaterialInstance)
	{
		PostProcessMaterialInstance->SetVectorParameterValue("OutsideColor", VignetteColor);
	}
}
