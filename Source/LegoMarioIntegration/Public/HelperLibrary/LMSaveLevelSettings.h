// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "LMSaveLevelSettings.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EDifficultyPattern : uint8
{
	EDP_Best,
	EDP_XBest,
	EDP_Average,
	EDP_XWorst,
	EDP_Worst,
	EDP_NONE,
};

UENUM(BlueprintType)
enum class ERowState : uint8
{
	ERS_Completed,
	ERS_Uncompleted,
	ERS_NONE
};

USTRUCT(BlueprintType)
struct FSaveRowState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="PixelSave", BlueprintReadOnly)
	TMap<int, ERowState> RowStateMap;
};

UENUM(BlueprintType)
enum class ELegoTower : uint8
{
	ELT_Mario,
	ELT_Luigi,
	ELT_Both,
	ERS_NONE
};

USTRUCT(BlueprintType)
struct FHitInfos
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="LevelSave", BlueprintReadWrite)
	TArray<uint8> HitHistory;
	UPROPERTY(VisibleAnywhere, Category="LevelSave", BlueprintReadWrite)
	TArray<float> ExposureTime;
	UPROPERTY(VisibleAnywhere, Category="LevelSave", BlueprintReadWrite)
	TArray<float> HitTime;

	FHitInfos& operator+=(const FHitInfos& Value);
};

USTRUCT(BlueprintType)
struct FPerMoleHit
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, Category="LevelSave", BlueprintReadOnly)
	TMap<FString, FHitInfos> PerMoleHitMap;
};

USTRUCT(BlueprintType)
struct FPerLegoHit
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, Category="LevelSave", BlueprintReadOnly)
	TMap<ELegoTower, FPerMoleHit> PerLegoHitMap;
	UPROPERTY(VisibleAnywhere, Category="LevelSave", BlueprintReadOnly)
	TArray<uint8> Streak = TArray<uint8>();
};

UCLASS()
class LEGOMARIOINTEGRATION_API ULMSaveLevelSettings : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="PixelSave", BlueprintReadOnly)
	TMap<FGameplayTag, FSaveRowState> UncompletedRows;

	UPROPERTY(VisibleAnywhere, Category="DDAProperties", BlueprintReadOnly)
	TMap<FGameplayTag, FPerLegoHit> PerLevelHitMap;

	UPROPERTY(VisibleAnywhere, Category="LevelSave", BlueprintReadOnly)
	float NextSpawnTime = 0.1f;

	UPROPERTY(VisibleAnywhere, Category="DDAProperties", BlueprintReadOnly)
	FString PixelArtsJsonString;

	UFUNCTION(BlueprintCallable, Category="DDAProperties")
	static void HitInfoBest(FHitInfos Result, UPARAM(ref)float& Hit, UPARAM(ref)float& Exposure);

	UFUNCTION(BlueprintCallable, Category="DDAProperties")
	static void HitInfoXBest(FHitInfos Result, UPARAM(ref)float& Hit, UPARAM(ref)float& Exposure, const int XIndex);

	UFUNCTION(BlueprintCallable, Category="DDAProperties")
	static void HitInfoAverage(FHitInfos Result, UPARAM(ref)float& Hit, UPARAM(ref)float& Exposure);

	UFUNCTION(BlueprintCallable, Category="DDAProperties")
	static void HitInfoXWorst(FHitInfos Result, UPARAM(ref)float& Hit, UPARAM(ref)float& Exposure, const int XIndex);

	UFUNCTION(BlueprintCallable, Category="DDAProperties")
	static void HitInfoWorst(FHitInfos Result, UPARAM(ref)float& Hit, UPARAM(ref)float& Exposure);

	UFUNCTION(BlueprintCallable)
	void FillUncompletedRowsWithTag(const FGameplayTag& PixelArtTag, int CurrentRow, const ERowState& RowState);
	/// Helper Function responsible for In-Game HitInfo cache of current Tower Hit or not.
	/// @param HitMapToFill PlayerState HitInfos to fill with given data
	/// @param PixelArtTag Current Level Pixel Tag
	/// @param TowerType Whether it was Mario or Luigi's turn 
	/// @param Seed Seed of the current Tower
	/// @param Exposure Per tower ExposureTime
	/// @param bHasHit Whether the tower has been hit or not
	/// @param HitTime HitTime in case it was hit, otherwise will equal Per tower ExposureTime 
	UFUNCTION(BlueprintCallable)
	static void FillHitInfo(TMap<FGameplayTag, FPerLegoHit>& HitMapToFill, const FGameplayTag& PixelArtTag,
	                        const ELegoTower TowerType, const FString& Seed, const float Exposure, const bool bHasHit,
	                        const float HitTime);
	/// Helper Function responsible for initially setting Per Tower Hit Infos for later difficulty usage
	/// @param TowerSeed Current Tower Seed
	/// @param Result HitInfos gather for the current Tower Seed
	/// @param PerLegoHit Filter whether the Result is in destination of Mario or Luigi
	void FillHitInfos(const FString& TowerSeed, FHitInfos& Result, const TTuple<ELegoTower, FPerMoleHit>& PerLegoHit) const;
	/// Responsible for Saving the current row result into hard drive. Happens once every row ends.
	/// @param HitMapToSave PlayerState HitInfos from current row
	/// @return Save File populated class
	UFUNCTION(BlueprintCallable)
	ULMSaveLevelSettings* FillHitInfoSave(UPARAM(ref)TMap<FGameplayTag, FPerLegoHit>& HitMapToSave);
	/// Return the Difficulty Pattern based on saved infos
	/// @param PixelArtTag Current Level Pixel Tag (62 possibilities)
	/// @return Difficulty Pattern
	UFUNCTION(BlueprintCallable)
	EDifficultyPattern GetDifficultyLevel(const FGameplayTag& PixelArtTag);
	/// Get the last Row Player is allowed to play for subLevel management (ERS_NONE).
	/// @param PixelArtTag Current Level Pixel Tag (62 possibilities)
	/// @return 0 to Current PixelArt maximum Rows
	UFUNCTION(BlueprintCallable)
	int GetLastAllowedRowsByTag(const FGameplayTag& PixelArtTag);

	/// Responsible for Profile display within MainMenu widget combo box.
	/// @return return the array of all .sav files located in Saved Folder
	UFUNCTION(BlueprintCallable)
	static TArray<FString> GetLevelSavefiles();
	/// Responsible to request Data from FillHitInfos for later difficulty Usage based on filters
	/// @param Result Out result Per Lego
	/// @param LevelTag Current Level Pixel Tag (62 possibilities)
	/// @param LegoTower Filter Lego
	/// @param TowerSeed Filter Seed
	UFUNCTION(BlueprintCallable, meta=(Categories="PixelArt"))
	void RequestHitInfoDatas(FHitInfos& Result, const FGameplayTag LevelTag, const ELegoTower LegoTower = ELegoTower::ELT_Both, const FString& TowerSeed = "ALL") const;
	/// X represent up to half the values available for either XBest or XWorst Patterns, having at least 10 % of total values so that we are not sticking to one result that may rely on lucky hits, based on total wins and defeats.
	/// @param LevelTag Current Level Pixel Tag (62 possibilities)
	/// @return Returns how many Values out of Difficulty partterns should be used to generate per Tower HitInfos
	UFUNCTION(BlueprintCallable, meta=(Categories="PixelArt"))
	float GetXPercentage(const FGameplayTag LevelTag) const;
};
