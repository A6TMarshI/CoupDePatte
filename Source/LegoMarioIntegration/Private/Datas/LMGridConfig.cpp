// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/LMGridConfig.h"

const FGridInfo* ULMGridConfig::FindGridInfo(const FGameplayTag& TileShapeTag) const
{
	for (const auto& GridInfo : GridInfos)
	{
		if(GridInfo.TileShape==TileShapeTag)
		{
			return &GridInfo;
		}
	}
	return nullptr;
}
