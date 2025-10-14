// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LMAccelerometerWidget.h"
#include "LMMatchInfoWidget.h"
#include "LMOptionsMenu.h"
#include "LMPostMatchWidget.h"
#include "LMStartmatchCountdownWidget.h"
#include "GameFramework/HUD.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "LMMarioHUD.generated.h"

/**
 * 
 */
UCLASS()
class LEGOMARIOINTEGRATION_API ALMMarioHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ULMAccelerometerWidget* DebugOverlay;
	UPROPERTY(BlueprintReadOnly)
	ULMMatchInfoWidget* MatchOverlay;
	UPROPERTY()
	ULMStartmatchCountdownWidget* MatchStartingOverlay;
	UPROPERTY()
	ULMOptionsMenu* PauseOptionsOverlay;
	UPROPERTY()
	ULMPostMatchWidget* PostMatchOverlay;
	UPROPERTY(BlueprintReadOnly)
	UPAPixelArtWidget* PixelArtOverlay;

	void AddAccelerometerOverlay(int PlayerIndex);
	void AddMatchInfoOverlay();
	void AddPixelArtOverlay();
	void AddMatchStartingCountdown();
	UFUNCTION(BlueprintCallable)
	void AddPauseOptionsOverlay();
	void HidePauseOptionsOverlay();
	void SetMatchInfoOverlayVisibility(const ESlateVisibility& Visibility);
	void SetPixelRowVisibility(const ESlateVisibility& Visibility);
	void SetAccelerometerOverlayVisibility(const ESlateVisibility& Visibility);
	void SetPixelArtOverlayVisibility(const ESlateVisibility& Visibility);
	void AddPostMatchOverlay();
	void ClearAllHUD();
	UFUNCTION(BlueprintCallable)
	void SetLayoutByTrainedHand(const EHandTrained& HandTrained);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<ULMAccelerometerWidget> AccelerometerClass;
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<ULMMatchInfoWidget> MatchOverlayClass;
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<ULMStartmatchCountdownWidget> MatchStartingCountdownClass;
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<ULMOptionsMenu> PauseOptionsClass;
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<ULMPostMatchWidget> PostMatchOverlayClass;
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UPAPixelArtWidget> PixelArtClass;
};
