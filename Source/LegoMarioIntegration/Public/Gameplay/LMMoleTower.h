// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "HelperLibrary/LMSaveLevelSettings.h"
#include "Tags/LMGameplayTags.h"
#include "LMMoleTower.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoleKilledDelegate);

UCLASS()
class LEGOMARIOINTEGRATION_API ALMMoleTower : public AActor
{
	GENERATED_BODY()

public:
	ALMMoleTower();

	void SetTowerColor(FLinearColor NewColor);

	/// Activate the current Tower and set it ready to receive damages. Track when it was activated for later Data saves.
	/// @param FailedSpawnModifier Gamemode defined value for allowing greater exposure time Loose streak
	/// @param PlayerTurn Current LEGO's turn between Mario or Luigi 
	/// @return Whether the spawn has been successful or not
	UFUNCTION(BlueprintCallable)
	bool SpawnMole(const float FailedSpawnModifier, const APlayerController* PlayerTurn);

	/// Reset Tower activation
	///
	void KillMole();

	/// Set activation time based on a CurveTimeLine from old system which requires world movements
	/// @param Length Calculated Exposure Time
	void SetCurveLength(const float& Length);
	/// Gather Values from LevelSave for the current level based on the Gamemode defined Difficulty Pattern for the current Tower for later usage
	/// @param Pattern Current Gamemode defined Difficulty Pattern based on LevelSave Data <see cref="GetDifficultyLevel\">
	/// @param LevelSave Current LevelSave for the Player
	/// @param LevelPixelTag Current Level Pixel Tag (62 Possibilities)
	/// @param HitInfos Per Lego Hit Info <see cref="GetHitInfoDatas\">
	/// @param TriggerTime TriggerTime given from LevelSave Data, based on the selected difficulty range (Raw without any Weighting apply)
	/// @param ExposureTime ExposureTime given from LevelSave Data, based on the selected difficulty range (Raw without any Weighting apply)
	void ProcessPerLegoSpawnInfos(EDifficultyPattern Pattern, ULMSaveLevelSettings* LevelSave, const FGameplayTag& LevelPixelTag, FHitInfos& HitInfos, float& TriggerTime, float& ExposureTime);
	/// Cache the values from LevelSave for the current level based on the Gamemode defined Difficulty Pattern for the current Tower for later usage
	/// @param Pattern Current Gamemode defined Difficulty Pattern based on LevelSave Data <see cref="GetDifficultyLevel\">
	/// @param LevelSave Current LevelSave for the Player
	/// @param LevelPixelTag Current Level Pixel Tag (62 Possibilities)
	void SetTowerSpawnInfos(EDifficultyPattern Pattern, ULMSaveLevelSettings* LevelSave, const FGameplayTag& LevelPixelTag);

	FMoleKilledDelegate OnMoleReadyToReSpawn;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	UNiagaraComponent* InitializeNiagara(UNiagaraSystem* Particules, FVector Size);
	UFUNCTION(BlueprintCallable)
	void SetTowerBlink();
	/// Apply the Weighting Logarithm Function over the cached TriggerTime and ExposureTime for smoother user difficulty progression
	/// @param FailedSpawnModifier Gamemode defined Value in case of Loose Streak
	/// @param PlayerTurn Current LEGO's turn between Mario or Luigi 
	void ApplyCalculatedTime(float FailedSpawnModifier, const APlayerController* PlayerTurn);

private:
	UPROPERTY(VisibleAnywhere, Category="MeshProperties")
	UStaticMeshComponent* TowerMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="MeshProperties")
	UStaticMeshComponent* TowerStandComponent;

	UPROPERTY(EditAnywhere, Category="SpawnProperties")
	UNiagaraSystem* SpawnParticules;

	UPROPERTY(EditAnywhere, Category="SpawnProperties")
	UNiagaraSystem* KillParticules;

	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	UNiagaraComponent* SpawnedParticules;

	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	UNiagaraComponent* KilledParticules;

	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	UTimelineComponent* OrcTimeline;

	UPROPERTY(EditAnywhere, Category="SpawnProperties")
	UCurveFloat* TimeLineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpawnProperties", meta=(AllowPrivateAccess=true))
	int DDAThreshold = 3;

	UPROPERTY(EditAnywhere, Category="SoundProperties")
	USoundBase* SpawnSound;

	UPROPERTY(EditAnywhere, Category="SoundProperties")
	USoundBase* DamageSound;

	UFUNCTION()
	void UpdateOrcPosition(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	UPROPERTY()
	double SpawnedTime = 0;

	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	bool bCanReceiveDamage = false;
	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	bool bWaitForReloadToSpawn = true;


	bool bMoleOut = false;
	UPROPERTY(EditAnywhere, Category="SpawnProperties")
	bool bIsTowerActivated = false;

	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	FString TowerSeed = FString();

	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	float MarioExposureTime = 0;
	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	float MarioTriggerTime = 0;

	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	float LuigiExposureTime = 0;
	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	float LuigiTriggerTime = 0;

	UPROPERTY(VisibleAnywhere, Category="SpawnProperties")
	float CalculatedExposureTime = 0;

	UPROPERTY(VisibleAnywhere, Category="MeshProperties")
	FLinearColor TowerColor = FLinearColor();

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FLinearColor GetTowerColor() const { return TowerColor; }

	FORCEINLINE void SetWaitForReloadToSpawn(const bool& WaitForReloadToSpawn) { bWaitForReloadToSpawn = WaitForReloadToSpawn; }
	FORCEINLINE void SetTowerSeed(const FString& Seed) { TowerSeed = Seed; }
	FORCEINLINE bool CanReceiveDamage() const { return bCanReceiveDamage; }
	FORCEINLINE bool IsMoleOut() const { return bMoleOut; }
	FORCEINLINE bool IsTowerActivated() const { return bIsTowerActivated; }
	FORCEINLINE void SetTowerActivated(const bool& bActivated)
	{
		bIsTowerActivated = bActivated;
		TowerMeshComponent->SetVisibility(bIsTowerActivated);
	}

	FORCEINLINE UStaticMeshComponent* GetTowerMeshComponent() const { return TowerMeshComponent; }
	FORCEINLINE UStaticMeshComponent* GetTowerStandComponent() const { return TowerStandComponent; }
	FORCEINLINE UTimelineComponent* GetTimeLine() const { return OrcTimeline; }
	FORCEINLINE double GetSpawnedTime() const { return SpawnedTime; }
	// FORCEINLINE double GetNextSpawnTime() const { return NextCalculatedSpawnTime; }
	FORCEINLINE double GetExposureTime() const { return CalculatedExposureTime; }

	FString GetTowerSeed() const { return TowerSeed; }
	float GetTopLocation() const;
};
