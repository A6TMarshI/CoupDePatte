// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "PixelArt/Public/HUD/PAPixelArtWidget.h"
#include "Components/TextBlock.h"
#include "LMMatchInfoWidget.generated.h"

class UImage;
class UHorizontalBox;
class ALMWhackAMoleGameMode;
/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ULMMatchInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	void SetCurrentPixelOutline();

	UFUNCTION()
	void HandleOnVisibilityChanged(ESlateVisibility InVisibility);
	UFUNCTION()
	void OnPixelChanged(bool bHit, FLinearColor TowerColor);
	void SetLastPixelColor(bool bHit, FLinearColor TowerColor);

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_MatchInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UBorder* B_PixelRow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UBorder* B_CurrentPixelRow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* BTN_Pause;

private:
	UPROPERTY()
	ALMWhackAMoleGameMode* WhackAMoleGameMode;
	UPROPERTY()
	UHorizontalBox* PixelRow;
	UPROPERTY()
	UImage* CurrentPixelImage;
	UPROPERTY(EditAnywhere, Category="Pixel")
	UMaterialInstance* OutlineMaterial;
	UPROPERTY(EditAnywhere, Category="Pixel")
	UMaterialInstance* MissedMaterial;

	/*UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* TXT_MatchTimer;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* PB_Timer;*/
};
