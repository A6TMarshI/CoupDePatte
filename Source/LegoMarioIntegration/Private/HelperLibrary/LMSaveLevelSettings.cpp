// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperLibrary/LMSaveLevelSettings.h"

#include "Gameplay/Tags/LMGameplayTags.h"
#include "Kismet/KismetArrayLibrary.h"

FHitInfos& FHitInfos::operator+=(const FHitInfos& Value)
{
	for (auto Exposure : Value.ExposureTime)
	{
		ExposureTime.Add(Exposure);
	}
	for (auto Hit : Value.HitHistory)
	{
		HitHistory.Add(Hit);
	}
	for (auto Hit : Value.HitTime)
	{
		HitTime.Add(Hit);
	}
	return *this;
}

void ULMSaveLevelSettings::HitInfoBest(FHitInfos Result, float& Hit, float& Exposure)
{
	Algo::Sort(Result.ExposureTime);
	Algo::Sort(Result.HitTime);
	Exposure = Result.ExposureTime.IsEmpty() ? 0 : Result.ExposureTime[0];
	Hit = Result.HitTime.IsEmpty() ? 0 : Result.HitTime[0];
}

void ULMSaveLevelSettings::HitInfoXBest(FHitInfos Result, float& Hit, float& Exposure, int XIndex)
{
	Algo::Sort(Result.ExposureTime);
	Algo::Sort(Result.HitTime);
	for (int i = 0; i < XIndex; i++)
	{
		Exposure += Result.ExposureTime[i];
	}
	for (int i = 0; i < XIndex; i++)
	{
		Hit += Result.HitTime[i];
	}
	Exposure /= XIndex;
	Hit /= XIndex;
}

void ULMSaveLevelSettings::HitInfoAverage(FHitInfos Result, float& Hit, float& Exposure)
{
	for (auto E : Result.ExposureTime)
	{
		Exposure += E;
	}
	for (auto H : Result.HitTime)
	{
		Hit += H;
	}
	Exposure /= Result.ExposureTime.Num();
	Hit /= Result.HitTime.Num();
}

void ULMSaveLevelSettings::HitInfoXWorst(FHitInfos Result, float& Hit, float& Exposure, const int XIndex)
{
	Algo::Sort(Result.ExposureTime);
	Algo::Sort(Result.HitTime);
	for (int i = Result.ExposureTime.Num() - XIndex; i < Result.ExposureTime.Num(); i++)
	{
		Exposure += Result.ExposureTime[i];
	}
	for (int i = Result.HitTime.Num() - XIndex; i < Result.HitTime.Num(); i++)
	{
		Hit += Result.HitTime[i];
	}
	Exposure /= XIndex;
	Hit /= XIndex;
}

void ULMSaveLevelSettings::HitInfoWorst(FHitInfos Result, float& Hit, float& Exposure)
{
	Algo::Sort(Result.ExposureTime);
	Algo::Sort(Result.HitTime);
	Exposure = Result.ExposureTime.IsEmpty() ? 0 : Result.ExposureTime[Result.ExposureTime.Num() - 1];
	Hit = Result.HitTime.IsEmpty() ? 0 : Result.HitTime[Result.HitTime.Num() - 1];
}

void ULMSaveLevelSettings::FillUncompletedRowsWithTag(const FGameplayTag& PixelArtTag, int CurrentRow,
                                                      const ERowState& RowState)
{
	if (!PixelArtTag.RequestDirectParent().MatchesTag(TAG_PixelArt))return;

	if (UncompletedRows.Contains(PixelArtTag))
	{
		if (UncompletedRows[PixelArtTag].RowStateMap.Contains(CurrentRow))
			UncompletedRows[PixelArtTag].RowStateMap[
				CurrentRow] = RowState;
		else UncompletedRows[PixelArtTag].RowStateMap.Add(CurrentRow, RowState);
	}
	else
	{
		FSaveRowState CurrentRowStateMap;
		CurrentRowStateMap.RowStateMap.Add(CurrentRow, RowState);
		UncompletedRows.Add(PixelArtTag, CurrentRowStateMap);
	}
}

void ULMSaveLevelSettings::FillHitInfos(const FString& TowerSeed, FHitInfos& Result, const TTuple<ELegoTower, FPerMoleHit>& PerLegoHit) const
{
	for (auto PerMoleHit : PerLegoHit.Value.PerMoleHitMap)
	{
		if (PerMoleHit.Key.Equals(TowerSeed, ESearchCase::IgnoreCase) || TowerSeed.Equals("ALL", ESearchCase::IgnoreCase))Result += PerMoleHit.Value;
	}
}

void ULMSaveLevelSettings::FillHitInfo(TMap<FGameplayTag, FPerLegoHit>& HitMapToFill,
                                       const FGameplayTag& PixelArtTag, const ELegoTower TowerType, const FString& Seed,
                                       const float Exposure, const bool bHasHit, const float HitTime)
{
	if (HitMapToFill.Contains(PixelArtTag))
	{
		if (HitMapToFill[PixelArtTag].PerLegoHitMap.Contains(TowerType))
		{
			if (HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap.Contains(Seed))
			{
				HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].ExposureTime.Add(Exposure);
				HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].HitHistory.Add(bHasHit);
				HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].HitTime.Add(HitTime);
			}
			else
			{
				HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap.Add(Seed);
				HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].ExposureTime.Add(Exposure);
				HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].HitHistory.Add(bHasHit);
				HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].HitTime.Add(HitTime);
			}
		}
		else
		{
			HitMapToFill[PixelArtTag].PerLegoHitMap.Add(TowerType);
			HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap.Add(Seed);
			HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].ExposureTime.Add(Exposure);
			HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].HitHistory.Add(bHasHit);
			HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].HitTime.Add(HitTime);
		}
	}
	else
	{
		HitMapToFill.Add(PixelArtTag);
		HitMapToFill[PixelArtTag].PerLegoHitMap.Add(TowerType);
		HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap.Add(Seed);
		HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].ExposureTime.Add(Exposure);
		HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].HitHistory.Add(bHasHit);
		HitMapToFill[PixelArtTag].PerLegoHitMap[TowerType].PerMoleHitMap[Seed].HitTime.Add(HitTime);
	}
}

ULMSaveLevelSettings* ULMSaveLevelSettings::FillHitInfoSave(TMap<FGameplayTag, FPerLegoHit>& HitMapToSave)
{
	for (auto PerLevelHit : HitMapToSave)
	{
		if (!PerLevelHitMap.Contains(PerLevelHit.Key))
		{
			PerLevelHitMap.Add(PerLevelHit);
			continue;
		}
		for (auto Streak : PerLevelHit.Value.Streak)
		{
			PerLevelHitMap[PerLevelHit.Key].Streak.Add(Streak);
		}
		for (auto PerLegoHit : PerLevelHit.Value.PerLegoHitMap)
		{
			if (!PerLevelHitMap[PerLevelHit.Key].PerLegoHitMap.Contains(PerLegoHit.Key))
			{
				PerLevelHitMap[PerLevelHit.Key].PerLegoHitMap.Add(PerLegoHit);
				continue;
			}
			for (auto PerMoleHit : PerLevelHit.Value.PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap)
			{
				if (!PerLevelHitMap[PerLevelHit.Key].PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap.Contains(PerMoleHit.Key))
				{
					PerLevelHitMap[PerLevelHit.Key].PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap.Add(PerMoleHit);
					continue;
				}
				for (auto Exposure : PerLevelHit.Value.PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap[PerMoleHit.Key].ExposureTime)
				{
					PerLevelHitMap[PerLevelHit.Key].PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap[PerMoleHit.Key].ExposureTime.Add(Exposure);
				}
				for (auto Trigger : PerLevelHit.Value.PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap[PerMoleHit.Key].HitHistory)
				{
					PerLevelHitMap[PerLevelHit.Key].PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap[PerMoleHit.Key].HitHistory.Add(Trigger);
				}
				for (auto Hit : PerLevelHit.Value.PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap[PerMoleHit.Key].HitTime)
				{
					PerLevelHitMap[PerLevelHit.Key].PerLegoHitMap[PerLegoHit.Key].PerMoleHitMap[PerMoleHit.Key].HitTime.Add(Hit);
				}
			}
		}
	}
	HitMapToSave.Empty();
	return this;
}

EDifficultyPattern ULMSaveLevelSettings::GetDifficultyLevel(const FGameplayTag& PixelArtTag)
{
	if (!PerLevelHitMap.Contains(PixelArtTag))return EDifficultyPattern::EDP_NONE;
	int TotalGame = PerLevelHitMap[PixelArtTag].Streak.Num();
	int TotalWin = 0;
	int TotalLoss = 0;
	int TotalHit = 0;
	int TotalFailed = 0;
	int Streak = 0;
	bool bIsWinStreak = false;
	for (auto Value : PerLevelHitMap[PixelArtTag].Streak)
	{
		if (Value == 1)
		{
			++TotalWin;
			if (!bIsWinStreak)
			{
				bIsWinStreak = true;
				Streak = 0;
			}
		}
		else if (Value == 0)
		{
			++TotalLoss;
			if (bIsWinStreak)
			{
				bIsWinStreak = false;
				Streak = 0;
			}
		}
		++Streak;
	}
	for (auto LegoHit : PerLevelHitMap[PixelArtTag].PerLegoHitMap)
	{
		for (auto TowerHit : LegoHit.Value.PerMoleHitMap)
		{
			for (auto HitValue : TowerHit.Value.HitHistory)
			{
				HitValue ? ++TotalHit : ++TotalFailed;
			}
		}
	}

	float WinRate = static_cast<float>(TotalWin) / static_cast<float>(TotalGame);
	float HitRate = static_cast<float>(TotalHit) / static_cast<float>(TotalFailed + TotalHit);
	if (WinRate > .85f && HitRate >= .9f && bIsWinStreak && Streak >= 2)return EDifficultyPattern::EDP_XBest;
	if (WinRate > .75f && HitRate >= .8f)
	{
		if (!bIsWinStreak)
		{
			if (Streak == 2)return EDifficultyPattern::EDP_XWorst;
			if (Streak >= 3)return EDifficultyPattern::EDP_Worst;
		}
		return EDifficultyPattern::EDP_Average;
	}
	if (!bIsWinStreak)return EDifficultyPattern::EDP_Worst;
	return EDifficultyPattern::EDP_XWorst;
}

int ULMSaveLevelSettings::GetLastAllowedRowsByTag(const FGameplayTag& PixelArtTag)
{
	if (!UncompletedRows.Contains(PixelArtTag) || UncompletedRows[PixelArtTag].RowStateMap.IsEmpty())return INDEX_NONE;
	int i = 0;
	for (i; i < UncompletedRows[PixelArtTag].RowStateMap.Num(); i++)
	{
		if (UncompletedRows[PixelArtTag].RowStateMap[i] == ERowState::ERS_NONE) return i;
	}
	return i;
}

TArray<FString> ULMSaveLevelSettings::GetLevelSavefiles()
{
	TArray<FString> SaveGameFiles;
	FString SaveGamePath = FPaths::ProjectSavedDir() / TEXT("SaveGames");

	IFileManager& FileManager = IFileManager::Get();
	FileManager.FindFiles(SaveGameFiles, *SaveGamePath, TEXT(".sav"));

	return SaveGameFiles;
}

void ULMSaveLevelSettings::RequestHitInfoDatas(FHitInfos& Result, const FGameplayTag LevelTag, const ELegoTower LegoTower, const FString& TowerSeed) const
{
	for (auto PerLevelHit : PerLevelHitMap)
	{
		if (LevelTag.IsValid() && !PerLevelHit.Key.MatchesTag(LevelTag) && !PerLevelHit.Key.RequestDirectParent().MatchesTag(LevelTag))continue;
		for (auto PerLegoHit : PerLevelHit.Value.PerLegoHitMap)
		{
			switch (LegoTower)
			{
			case ELegoTower::ELT_Both:
				FillHitInfos(TowerSeed, Result, PerLegoHit);
				break;
			case ELegoTower::ELT_Luigi:
				if (PerLegoHit.Key != LegoTower)continue;
				FillHitInfos(TowerSeed, Result, PerLegoHit);
				break;
			case ELegoTower::ELT_Mario:
				if (PerLegoHit.Key != LegoTower)continue;
				FillHitInfos(TowerSeed, Result, PerLegoHit);
				break;
			default:
				break;
			}
		}
	}
}

float ULMSaveLevelSettings::GetXPercentage(const FGameplayTag LevelTag) const
{
	if (!PerLevelHitMap.Contains(LevelTag))return 0;
	int TotalWin = 0;
	for (auto S : PerLevelHitMap[LevelTag].Streak)
	{
		if (S == 1)TotalWin++;
	}
	return 0.5f * (1.f - FMath::Abs(2.f * static_cast<float>(TotalWin) / static_cast<float>(PerLevelHitMap[LevelTag].Streak.Num()) - 1));
}
