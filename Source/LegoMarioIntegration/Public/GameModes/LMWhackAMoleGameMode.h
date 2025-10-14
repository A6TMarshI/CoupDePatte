// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/LMLayoutTowers.h"
#include "GameModes/LMTopDownGameMode.h"
#include "Gameplay/LMLegoTower.h"
#include "Gameplay/LMMoleTower.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "HelperLibrary/LMSaveLevelSettings.h"
#include "HUD/PAPixelCreationWidget.h"
#include "PlayerControllers/LMWhackAMoleController.h"
#include "LMWhackAMoleGameMode.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReloadedLegoDelegate, ALMLegoTower*, LegoTower);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPixelChangedDelegate, bool, bHit, FLinearColor, TowerColor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestedReloadLevel, const FString&, OptionString);

namespace MatchState
{
	extern LEGOMARIOINTEGRATION_API const FName PlayGame;
}

UENUM(BlueprintType)
enum class EHandTrained : uint8
{
	EHT_Right,
	EHT_Left,
	EHT_NONE
};

USTRUCT()
struct FDebugScan
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<ALMMoleTower*> ScanTowers;
};

USTRUCT(BlueprintType)
struct FLevelPixelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Spawn", meta=(Categories="PixelArt"), BlueprintReadWrite)
	FGameplayTag LVLPixelTag = TAG_NONE;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite)
	int CurrentRow = 0;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite)
	TArray<FVector2D> CurrentRowPixelCoordinate = TArray<FVector2D>();
	UPROPERTY(VisibleAnywhere, Category="Spawn", meta=(AllowPrivateAccess), BlueprintReadWrite)
	// Define from within pixel art json rows
	int CurrentMoleRemaining = 0;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite)
	ETowerMovement TowerMovement = ETowerMovement::ETM_Empty;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite)
	ETowerAngle TowerAngle = ETowerAngle::ETA_Empty;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite)
	ETowerHeight TowerHeight = ETowerHeight::ETH_Empty;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite)
	EHandTrained HandTrained = EHandTrained::EHT_Right;
};

UCLASS()
class LEGOMARIOINTEGRATION_API ALMWhackAMoleGameMode : public ALMTopDownGameMode
{
	GENERATED_BODY()

public:
	/// Cache Currently spawned Towers.
	/// @param MoleTowerActors Spawned Tower array following the Grid layout for the current level
	void InitializeMoleTowers(const TArray<AActor*>& MoleTowerActors);
	void ClearMoleTowers();
	/// Setup timer for the next mole to spawn (User Defined Value), Computed with the current FailedSpawnModifier if applied. Then Spawn the mole.
	///
	UFUNCTION()
	void InitiateMoleSpawn();
	/// Cache all spawned Towers into its corresponding Mapping <see cref="ScanMoleTowersMap\">. A tower may be contained within multiple mapping E.G Corners are contained within all mappings.
	/// @param ScanShape One of 4 ScanPlayerTags 
	/// @param MoleTower Spawned Tower
	void GenerateScanningTowerArray(FGameplayTag ScanShape, ALMMoleTower* MoleTower);
	UPROPERTY()
	FReloadedLegoDelegate OnReloadedLegoDelegate;
	UPROPERTY(BlueprintAssignable)
	FPixelChangedDelegate OnPixelChangedDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnRequestedReloadLevel OnRequestedReloadLevelDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void EndMatch() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Tower")
	TArray<ALMMoleTower*> MoleTowers;
	UPROPERTY()
	TMap<FGameplayTag, FDebugScan> ScanMoleTowersMap;
	UPROPERTY(VisibleAnywhere, Category="Tower")
	TMap<APlayerController*, ALMLegoTower*> LegoTowers;

	FTimerHandle MoleSpawnTimerHandle;

	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite, meta=(AllowPrivateAccess))
	bool bWaitForReloadToSpawn = true;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite, meta=(AllowPrivateAccess))
	bool bRandomPlayerTurn = false;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite, meta=(AllowPrivateAccess))
	bool bLastGameWin = false;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float FailSpawnModifier = 1.4f;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float NextSpawnTime = 6.f;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite, meta=(AllowPrivateAccess))
	EDifficultyPattern DifficultyPattern = EDifficultyPattern::EDP_NONE;
	UPROPERTY(EditAnywhere, Category="Spawn")
	float MapScanTime = 7.f;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite, meta=(AllowPrivateAccess))
	float AllowedMissMoleModifier = 0;
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadWrite, meta=(AllowPrivateAccess))
	FLevelPixelInfo LevelPixelInfo = FLevelPixelInfo();
	UPROPERTY(EditAnywhere, Category="Spawn", BlueprintReadOnly, meta=(AllowPrivateAccess))
	FPAPixelArtDataInfos PixelArtDataInfos = FPAPixelArtDataInfos();
	int TotalMole = 0;
	float FailedSpawnDecrement = 0;

	/// Initiate mole spawn. Default behavior request a random Tower to activate. PlayerScan was a test mode where all towers were placed and only pattern generated activation like corners, Horizontal extremity, Vertical extremity and all line one after the other <see cref="ScanPlayerTag\">
	/// @return Whether the Spawn has been successful or not
	bool SpawnMole();

	UPROPERTY(VisibleAnywhere, Category="Spawn")
	ALMMoleTower* SpawnedMole;

	UPROPERTY(VisibleAnywhere, Category="Gameplay")
	APlayerController* ControllerToPlay;

	UPROPERTY()
	bool bSpawnEnabled = true;

	FTimerHandle StartMatchTimerHandle;
	UPROPERTY(EditAnywhere, Category="MatchSettings")
	int StartMatchTimeCountdown;
	UPROPERTY()
	int Health = 4;

protected:
	/// Handle Level restart whether we hit continue or restart on post match
	/// @param Key Value item to parse into Row Values. This step was made mostly in blueprint which is the reason this value is not used in the function.
	UFUNCTION()
	void HandleRequestedReloadLevel(const FString& OptionString);
	
	virtual void OnMatchStateSet() override;
	
	///  Responsible for triggering the base StartMatch
	/// @param Unused parameter for the sake of TextCommit delegate binding
	UFUNCTION()
	void StartMoleMatch(const FString& Name);

	/// Custom Match start which only showup a widget which will be responsible for the actual match start once parameters are selected. 
	///
	virtual void StartMatch() override;
	
	/// Set Data based on the current pixel art row
	/// 
	///
	void InitializeCurrentRowData();
	
	virtual void StartMatchCountdown(const FString& Name);
	/// Clear the old Tower Animation and set the new tower one either randomly or alternate. Also set the vignette accordingly
	///
	void SetPlayerTurn();
	UFUNCTION(BlueprintCallable)
	void HandlePlayGame();
	// Reload the Lego counter and set the world widget ready, happens when either Luigi or Mario is placed on its tower
	UFUNCTION()
	void ReloadLego(ALMLegoTower* LegoTower);
	int ReloadedLego = 0;
/// Save The Pixel Art level into to Json String of pixel arts array. If it exists, override it.
///
	void SavePixelArt();

private:
	UPROPERTY(EditAnywhere, Category="SoundProperties")
	USoundBase* MarioTurnSound;
	UPROPERTY(EditAnywhere, Category="SoundProperties")
	USoundBase* LuigiTurnSound;

	UPROPERTY(EditAnywhere, Category="ScanProperties")
	bool bScanPlayer = false;

	UPROPERTY(EditAnywhere, Category="ScanProperties", meta=(Categories="Scan"))
	FGameplayTag ScanPlayerTag = TAG_Scan_Corner;

	int MoleScanIndex = 0;

	/// Cache Lego Towers always having Mario one right sided to avoid having to switch physical layout around
	///
	void CacheLegoTowers();
	
	TArray<FPAPixelArtDataInfos>::SizeType FindByTag(TArray<FPAPixelArtDataInfos> Array, FGameplayTag Tag);

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ALMMoleTower* GetDamageableMole() const { return SpawnedMole; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetHealth()const { return Health; }
	FORCEINLINE TMap<APlayerController*, ALMLegoTower*> GetLegoTowerMap() const { return LegoTowers; }
	FORCEINLINE APlayerController* GetPlayerTurn() const { return ControllerToPlay; }
	FORCEINLINE bool DoScanPlayer() const { return bScanPlayer; }
	FORCEINLINE float GetNextSpawnTime() const { return NextSpawnTime; }
	FORCEINLINE bool GetWaitForReloadToSpawn() const { return bWaitForReloadToSpawn; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetLevelCompleted() const
	{
		return LevelPixelInfo.CurrentMoleRemaining <= 0;
	}

	FORCEINLINE float GetLastGameResultModifier() const
	{
		return bLastGameWin ? 1.f : 1.4f;
	}

	FORCEINLINE FLevelPixelInfo GetLevelPixelInfo() const { return LevelPixelInfo; }
	FORCEINLINE FPAPixelArtDataInfos GetPixelArtDataInfos() const { return PixelArtDataInfos; }
	/// If the mole has been hit, mark it as unlocked and populate the HitColor with the current plate color
	/// Then decrement the remaining pixel counter. If it reaches 0, end the match
	/// @param bHit Whether the mole has been successfully hit or this was called from the timer reaching end
	void SaveAndRequestNewMoleFromHit(const bool bHit);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetCurrentPixelIndex() const
	{
		return LevelPixelInfo.CurrentRowPixelCoordinate.Num() - LevelPixelInfo.CurrentMoleRemaining;
	}
	
	/// Get the offset from current level Pixel Infos to gather correct Seeds from datatables Based on the current Datatable Disposition
	/// If the datatables order was to be modified, this would not work anymore
	/// 
	/// 0-61 : 0 Degree|0cm|Flat -> Offset = 0
	/// 
	/// 62-123 : 0 Degree|Variable|Flat -> Offset = 1
	/// 
	/// 124-185 : 45 Degree|Variable|Extension  -> Offset = 2
	/// 
	/// 186-247 : 90 Degree|Variable|Extension  -> Offset = 3
	/// 
	/// 248-309 : 45 Degree|Variable|Supination -> Offset = 4
	/// 
	/// 310-371 : 90 Degree|Variable|Supination -> Offset = 5
	/// 
	/// 372-433 : 45 Degree|Variable|Both -> Offset = 6
	/// 
	/// 434-495 : 90 Degree|Variable|Both -> Offset = 7
	/// @return Offset from 0 to 7 based on current Level layout requirements
	UFUNCTION(BlueprintCallable)
	int GetDataLayoutOffset() const;
	void SetDifficultyPattern(const EDifficultyPattern Pattern) { DifficultyPattern = Pattern; }
	EDifficultyPattern GetDifficultyPattern() const { return DifficultyPattern; }
};
