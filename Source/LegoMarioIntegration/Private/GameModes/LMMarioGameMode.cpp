// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LMMarioGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GameInstances/LMGameInstance.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "HUD/LMMarioHUD.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/LMMarioController.h"
#include "PlayerStates/LMPlayerState.h"
#include "Socket/LMSocket.h"


ALMMarioGameMode::ALMMarioGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	bDelayedStart = true;
}

void ALMMarioGameMode::BeginPlay()
{
	Super::BeginPlay();

	OnAllControllerConnectedDelegate.AddDynamic(this, &ThisClass::StartMatch);

	// This was made in c++ to ensure Username timing reliability over any gamemodes 
	if (UGameplayStatics::HasOption(OptionsString, "Username"))
	{
		Username = UGameplayStatics::ParseOption(OptionsString, "Username");
	}

	SpawnSecondLegoPlayer();
}

void ALMMarioGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	if (MatchState == MatchState::WaitingPostMatch)
	{
		if (const auto GameInstance = Cast<ULMGameInstance>(UGameplayStatics::GetGameInstance(this)))
		{
			GameInstance->PauseThread(true);
		}
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ALMMarioController* MarioController = Cast<ALMMarioController>(*Iterator))
		{
			MarioController->OnMatchStateSet(MatchState);
		}
	}
}

void ALMMarioGameMode::SpawnSecondLegoPlayer()
{
	if (!MarioCharacterClass)return;
	const TSubclassOf<APlayerStart> ClassToFind = APlayerStart::StaticClass();
	TArray<AActor*> FoundPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundPlayerStarts);

	if (UGameplayStatics::GetNumPlayerControllers(GetWorld()) >= FoundPlayerStarts.Num())return;
	for (int i = 1; i < FoundPlayerStarts.Num(); i++)
	{
		const UWorld* World = GetWorld();
		if (!World)return;

		const FVector SpawnLocation = FoundPlayerStarts[i]->GetActorLocation();
		FRotator SpawnRotation = FoundPlayerStarts[i]->GetActorRotation();

		ALMMarioController* ControllerToPossess = Cast<ALMMarioController>(UGameplayStatics::CreatePlayer(World));

		if (!ControllerToPossess)return;
		ControllerToPossess->SpawnLocation = SpawnLocation;
	}
}