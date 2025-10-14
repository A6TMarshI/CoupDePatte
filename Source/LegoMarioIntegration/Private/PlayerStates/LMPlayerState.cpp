// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStates/LMPlayerState.h"

#include "GameModes/LMMarioGameMode.h"
#include "GameModes/LMTopDownGameMode.h"

void ALMPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ALMPlayerState::RemoveHealth(const int Removed)
{
	Health -= Removed;
}
