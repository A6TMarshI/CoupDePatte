// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Datas/LMLayoutTowers.h"

TArray<FString> ULMLayoutTowers::FindLayoutInfo(const FGameplayTag& LayoutTag,
                                                const TArray<FLayoutActivatedTowers*>& Array, const int Offset)
{
	for (int i = Offset * 62; i < (Offset + 1) * 62; i++)
	{
		if (Array[i]->LayoutTag.MatchesTag(LayoutTag))
		{
			return Array[i]->LayoutInfos;
		}
	}
	return TArray<FString>();
}

TArray<FGameplayTag> ULMLayoutTowers::GetLevelRelatedPixelTags(const TArray<FLayoutActivatedTowers*>& Array, const int LevelIndex)
{
	TArray<FGameplayTag> RelatedPixelTags;
	for (int i = LevelIndex; i < (LevelIndex + 7 * 62); i += 62)
	{
		RelatedPixelTags.Add(Array[i]->LayoutTag);
	}
	return RelatedPixelTags;
}

TArray<FGameplayTag> ULMLayoutTowers::K2_GetLevelRelatedPixelTags(const TArray<FLayoutActivatedTowers>& Array, const int LevelIndex)
{
	TArray<FGameplayTag> RelatedPixelTags;
	for (int i = LevelIndex; i < (LevelIndex + 7 * 62); i += 62)
	{
		RelatedPixelTags.Add(Array[i].LayoutTag);
	}
	return RelatedPixelTags;
}
