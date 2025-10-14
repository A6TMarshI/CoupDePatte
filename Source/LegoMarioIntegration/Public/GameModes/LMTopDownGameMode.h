// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PostProcessVolume.h"
#include "GameModes/LMMarioGameMode.h"
#include "LMTopDownGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ALMTopDownGameMode : public ALMMarioGameMode
{
	GENERATED_BODY()

protected:
	void AddAllowedColors();
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
	virtual void InitializePostProcess();
	virtual bool IsMatchInProgress() const override;

	FLinearColor GetNextColor(const bool bClearAllowedColors);

	virtual void SetVignetteColor(FLinearColor VignetteColor);

	UPROPERTY(VisibleAnywhere, Category="PostProcess")
	APostProcessVolume* PostProcessVolume;

	UPROPERTY(EditAnywhere, Category="PostProcess")
	UMaterialInstance* PostProcessMaterialInstanceClass;

	UPROPERTY(VisibleAnywhere, Category="PostProcess")
	UMaterialInstanceDynamic* PostProcessMaterialInstance;

	UPROPERTY(VisibleAnywhere, Category="Tower")
	TArray<FLinearColor> Colors;
};
