// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "LegoMarioIntegration/LegoMarioIntegration.h"
#include "PlayerControllers/LMMarioController.h"
#include "LMLegoTower.generated.h"

#define FOOT_HEIGHT 5.1

UCLASS()
class LEGOMARIOINTEGRATION_API ALMLegoTower : public AActor
{
	GENERATED_BODY()

public:
	ALMLegoTower();

	void SetTowerColor(FLinearColor NewColor);
	void ChangeFootStepsColor(const bool bCanAttack);
	void PlayAnimationOnPlayerTurn(const bool bPlay);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="MeshSettings")
	UStaticMeshComponent* TowerMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="MeshSettings")
	UStaticMeshComponent* FootMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="MeshSettings")
	FLinearColor TowerColor;

	UPROPERTY(EditAnywhere, Category="HUDProperties", BlueprintReadWrite, meta=(AllowPrivateAccess))
	UWidgetComponent* ReadyWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category="TurnProperties")
	UTimelineComponent* PlayerTurnTimeline;

	UPROPERTY(EditAnywhere, Category="TurnProperties")
	UCurveFloat* TimeLineCurve;

	UFUNCTION()
	void UpdateFootShakingPosition(float Value);

public:
	FORCEINLINE FLinearColor GetTowerColor() const { return TowerColor; }
	FORCEINLINE FString GetTowerName() const { return TowerColor == BROWN_COLOR ? FString("Mario") : FString("Luigi"); }
	FORCEINLINE UStaticMeshComponent* GetFootMeshComponent() const { return FootMeshComponent; }
	FORCEINLINE void SetReadyWidgetVisibility(const bool& bVisible) const
	{
		ReadyWidgetComponent->SetVisibility(bVisible);
	}

	void SetReadyWidgetText(bool bReady, const APlayerController* PlayerController) const;
};
