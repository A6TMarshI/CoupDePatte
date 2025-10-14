// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LMWhackAMoleGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "Gameplay/LMGrid.h"
#include "Gameplay/LMLegoTower.h"
#include "Gameplay/LMMoleTower.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "Helpers/PAReadWriteFile.h"
#include "HUD/LMMarioHUD.h"
#include "HUD/PAPixelArtWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/LMMarioController.h"
#include "PlayerControllers/LMWhackAMoleController.h"
#include "PlayerStates/LMPlayerState.h"
#include "Socket/LMSocket.h"
#include "WorldSettings/LMMoleWorldSettings.h"

namespace MatchState
{
	const FName PlayGame = FName(TEXT("PlayGame"));
}

void ALMWhackAMoleGameMode::InitializeMoleTowers(const TArray<AActor*>& MoleTowerActors)
{
	bool bClearColors = true;
	for (const auto MoleTowerActor : MoleTowerActors)
	{
		if (auto MoleTower = Cast<ALMMoleTower>(MoleTowerActor))
		{
			if (!MoleTower->IsTowerActivated())continue;
			MoleTowers.Add(MoleTower);
			MoleTower->SetTowerColor(GetNextColor(bClearColors));
			bClearColors = false;
		}
	}
}

void ALMWhackAMoleGameMode::ClearMoleTowers()
{
	for (const auto MoleTower : MoleTowers)
	{
		MoleTower->Destroy();
	}
	MoleTowers.Empty();
}

void ALMWhackAMoleGameMode::BeginPlay()
{
	Super::BeginPlay();

	OnReloadedLegoDelegate.AddDynamic(this, &ThisClass::ReloadLego);
	OnRequestedReloadLevelDelegate.AddDynamic(this, &ThisClass::HandleRequestedReloadLevel);

	//ShuffleColors();

	CacheLegoTowers();

	auto WorldSettings = Cast<ALMMoleWorldSettings>(GetWorldSettings());
	if (!WorldSettings)return;
	LevelPixelInfo.LVLPixelTag = WorldSettings->GetLevelPixelTag();

	if (auto LevelSave = Cast<ULMSaveLevelSettings>(UGameplayStatics::LoadGameFromSlot(Username, 0)))
	{
		const auto MultipleStruct = UPAReadWriteFile::JsonToMultipleStruct(LevelSave->PixelArtsJsonString);
		if (!MultipleStruct.IsEmpty())
		{
			for (auto Struct : MultipleStruct)
			{
				if (Struct.PixelArtTag.MatchesTag(LevelPixelInfo.LVLPixelTag))
				{
					PixelArtDataInfos = Struct;
					break;
				}
			}
		}
		else UPAPixelArtWidget::TryGetPixelFromJson(PixelArtDataInfos, LevelPixelInfo.LVLPixelTag);
	}

	if (const auto Grid = Cast<ALMGrid>(UGameplayStatics::GetActorOfClass(GetWorld(), ALMGrid::StaticClass())))
	{
		Grid->SpawnTowers(TAG_Data_Square, LevelPixelInfo.HandTrained, GetDataLayoutOffset());
	}
}

void ALMWhackAMoleGameMode::StartMatch()
{
	if (PixelArtDataInfos.PixelArtTag.IsValid() && !PixelArtDataInfos.PixelColorMap.IsEmpty())
	{
		InitializeCurrentRowData();
	}
	else if (UPAPixelArtWidget::TryGetPixelFromJson(PixelArtDataInfos, LevelPixelInfo.LVLPixelTag))
	{
		InitializeCurrentRowData();
	}
	if (auto* LegoController = Cast<ALMMarioController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (ALMMarioHUD* MarioHUD = Cast<ALMMarioHUD>(LegoController->GetHUD()))
		{
			MarioHUD->AddMatchStartingCountdown();
			const FInputModeUIOnly InputModeUIOnly;
			LegoController->SetInputMode(InputModeUIOnly);
			LegoController->SetShowMouseCursor(true);
			if (MarioHUD->MatchStartingOverlay)
			{
				if (MarioHUD->MatchStartingOverlay->OnNameCommittedDelegate.IsBound())
					MarioHUD->MatchStartingOverlay->
					          OnNameCommittedDelegate.Clear();
				MarioHUD->MatchStartingOverlay->OnNameCommittedDelegate.AddDynamic(this, &ThisClass::StartMoleMatch);
				if (bLastGameWin)
				{
					MarioHUD->MatchStartingOverlay->ConfirmStartGame();
				}
			}
		}
	}
}

void ALMWhackAMoleGameMode::InitializeCurrentRowData()
{
	LevelPixelInfo.CurrentMoleRemaining = 0;
	LevelPixelInfo.CurrentRowPixelCoordinate.Empty();
	for (int i = 0; i < PixelArtDataInfos.Columns; i++)
	{
		FVector2D PixelCoordinate = FVector2D((PixelArtDataInfos.Rows - 1) - LevelPixelInfo.CurrentRow, i);
		if (PixelArtDataInfos.PixelColorMap.Contains(PixelCoordinate) && PixelArtDataInfos.PixelColorMap[
			PixelCoordinate].PixelState != EPixelState::EPS_Hidden)
		{
			LevelPixelInfo.CurrentMoleRemaining++;
			LevelPixelInfo.CurrentRowPixelCoordinate.Add(PixelCoordinate);
		}
	}
	TotalMole = LevelPixelInfo.CurrentMoleRemaining;
	Health = LevelPixelInfo.CurrentRowPixelCoordinate.Num() - FMath::FloorToInt(LevelPixelInfo.CurrentRowPixelCoordinate.Num() * AllowedMissMoleModifier);
	int ComputeMoleNumber = FMath::Max(FMath::Floor(TotalMole * .4f), 1);
	FailedSpawnDecrement = (FailSpawnModifier - 1.f) / ComputeMoleNumber;
}

void ALMWhackAMoleGameMode::StartMatchCountdown(const FString& Name)
{
	if (PixelArtDataInfos.PixelArtTag.IsValid() && !PixelArtDataInfos.PixelColorMap.IsEmpty())
	{
		InitializeCurrentRowData();
	}
	else if (UPAPixelArtWidget::TryGetPixelFromJson(PixelArtDataInfos, LevelPixelInfo.LVLPixelTag))
	{
		InitializeCurrentRowData();
	}
	const auto World = GetWorld();
	if (!World)return;
	if (auto* LegoController = Cast<ALMMarioController>(World->GetFirstPlayerController()))
	{
		if (const ALMMarioHUD* MarioHUD = Cast<ALMMarioHUD>(LegoController->GetHUD()))
		{
			if (MarioHUD->MatchStartingOverlay && MarioHUD->MatchStartingOverlay->TXT_Countdown && MarioHUD->
			                                                                                       MatchStartingOverlay->Anim_Countdown)
			{
				GetWorldTimerManager().SetTimer(StartMatchTimerHandle, [&, MarioHUD,LegoController]()
				{
					MarioHUD->MatchStartingOverlay->TXT_Countdown->SetText(
						FText::FromString(FString::Printf(TEXT("%s %d"), *FText::FromStringTable(STRING_TABLE_FR, FString("MatchStarting")).ToString(), StartMatchTimeCountdown)));
					MarioHUD->MatchStartingOverlay->PlayAnimation(MarioHUD->MatchStartingOverlay->Anim_Countdown);
					--StartMatchTimeCountdown;
					if (StartMatchTimeCountdown < 0)
					{
						GetWorldTimerManager().ClearTimer(StartMatchTimerHandle);
						MarioHUD->MatchStartingOverlay->RemoveFromParent();
						SetMatchState(MatchState::PlayGame);
					}
				}, 1.f, true);
			}
		}
	}
}

void ALMWhackAMoleGameMode::EndMatch()
{
	if (const auto PS = GetWorld()->GetFirstPlayerController()->GetPlayerState<ALMPlayerState>())
	{
		if (PS->HitMoleTelemetryInfo.Contains(LevelPixelInfo.LVLPixelTag))
			PS->HitMoleTelemetryInfo[LevelPixelInfo.LVLPixelTag].Streak.Add(GetLevelCompleted() && PS->GetHealth() >= 0);
	}
	Super::EndMatch();

	GetWorldTimerManager().PauseTimer(MoleSpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(MoleSpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(StartMatchTimerHandle);
	bSpawnEnabled = false;
}

bool ALMWhackAMoleGameMode::SpawnMole()
{
	if (!bSpawnEnabled || IsPaused())return false;
	if (!this || this != UGameplayStatics::GetGameMode(this))return false;
	auto MoleTowersToSpawn = bScanPlayer ? ScanMoleTowersMap[ScanPlayerTag].ScanTowers : MoleTowers;
	if (MoleTowersToSpawn.Num() > 0)
	{
		int TowerIndex = 0;
		if (bScanPlayer)
		{
			TowerIndex = MoleScanIndex++;
			if (MoleScanIndex >= ScanMoleTowersMap[ScanPlayerTag].ScanTowers.Num())
			{
				MoleScanIndex = 0;
				TArray<FGameplayTag> OutTags;
				ScanMoleTowersMap.GetKeys(OutTags);
				for (int i = 0; i < OutTags.Num(); i++)
				{
					UE_LOG(LogTemp, Warning, TEXT("%s"), *OutTags[i].ToString())
					if (ScanPlayerTag == OutTags[i])
					{
						ScanPlayerTag = OutTags[i == OutTags.Num() - 1 ? 0 : i + 1];
						break;
					}
				}
			}
		}
		else
		{
			do
			{
				TowerIndex = FMath::RandRange(0, MoleTowersToSpawn.Num() - 1);
			}
			while (!MoleTowersToSpawn.IsValidIndex(TowerIndex) || MoleTowersToSpawn[TowerIndex]->IsMoleOut());
		}
		SpawnedMole = MoleTowersToSpawn[TowerIndex];
		if (SpawnedMole->SpawnMole(FailSpawnModifier, GetPlayerTurn()))
		{
			return true;
		}
	}
	return false;
}

void ALMWhackAMoleGameMode::InitiateMoleSpawn()
{
	FailSpawnModifier = GetLastGameResultModifier();
	float ComputeSpawnTime = NextSpawnTime * FailSpawnModifier;
	GetWorldTimerManager().SetTimer(MoleSpawnTimerHandle, [this]
	{
		if (!IsValid(this) || !MoleSpawnTimerHandle.IsValid() || !IsValid(GetWorld()) || !GetWorld()->GetTimerManager().
		                                                                                              TimerExists(MoleSpawnTimerHandle) || GetWorld()->GetTimerManager().IsTimerPaused(MoleSpawnTimerHandle))
			return;
		if (!bSpawnEnabled)return;
		SetPlayerTurn();
		if (SpawnMole())
		{
			if (!SpawnedMole->OnMoleReadyToReSpawn.IsBound())
				SpawnedMole->OnMoleReadyToReSpawn.AddDynamic(
					this, &ThisClass::InitiateMoleSpawn);
		}
	}, ComputeSpawnTime, false);
	FailSpawnModifier -= FMath::IsNearlyEqual(FailSpawnModifier, 1.f, 0.05f) || FailSpawnModifier < 1 ? 0 : FailedSpawnDecrement;
}

void ALMWhackAMoleGameMode::SavePixelArt()
{
	TArray<FPAPixelArtDataInfos> PixelArtArray;
	if (auto LevelSave = Cast<ULMSaveLevelSettings>(UGameplayStatics::LoadGameFromSlot(Username, 0)))
	{
		if (!LevelSave->PixelArtsJsonString.IsEmpty())
		{
			PixelArtArray = UPAReadWriteFile::JsonToMultipleStruct(LevelSave->PixelArtsJsonString);
		}
		else
		{
			PixelArtArray = UPAReadWriteFile::ReadJsonToMultipleStruct(FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH);
		}

		const int Index = FindByTag(PixelArtArray, PixelArtDataInfos.PixelArtTag);
		if (Index != INDEX_NONE) PixelArtArray[Index] = PixelArtDataInfos;

		FString JsonString;
		UPAReadWriteFile::SerializeToTableJson(PixelArtArray, JsonString);
		if (JsonString.IsEmpty())return;
		LevelSave->PixelArtsJsonString = JsonString;
		UGameplayStatics::SaveGameToSlot(LevelSave, Username, 0);
	}
}

void ALMWhackAMoleGameMode::HandleRequestedReloadLevel(const FString& OptionString)
{
	SetMatchState(MatchState::WaitingToStart);
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ALMMarioController* MarioController = Cast<ALMMarioController>(*Iterator))
		{
			MarioController->ClearAllHUD();
			MarioController->ClearAllTags();
		}
	}
	if (const auto GameInstance = Cast<ULMGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GameInstance->PauseThread(false);
	}
	ReloadedLego = 0;
	StartMatchTimeCountdown = 3;
	LevelPixelInfo.CurrentMoleRemaining = 10;
	SetVignetteColor(FLinearColor::Transparent);
	StartMatch();
}

void ALMWhackAMoleGameMode::OnMatchStateSet()
{
	if (MatchState == MatchState::PlayGame)
	{
		bSpawnEnabled = true;
		InitiateMoleSpawn();
	}

	Super::OnMatchStateSet();

	if (MatchState == MatchState::InProgress)
	{
		for (const auto LegoTower : LegoTowers)
		{
			LegoTower.Value->SetReadyWidgetText(false, LegoTower.Key);
			if (LegoTower.Value)
			{
				LegoTower.Value->SetReadyWidgetVisibility(true);
			}
		}
	}
	if (MatchState == MatchState::WaitingPostMatch)
	{
		GetWorldTimerManager().PauseTimer(MoleSpawnTimerHandle);
		GetWorldTimerManager().ClearTimer(MoleSpawnTimerHandle);
		if (SpawnedMole && SpawnedMole->GetTimeLine()->IsPlaying())
		{
			SpawnedMole->GetTimeLine()->Stop();
		}
		for (auto LegoTower : LegoTowers)
		{
			LegoTower.Value->PlayAnimationOnPlayerTurn(false);
		}

		const auto PS = GetWorld()->GetFirstPlayerController()->GetPlayerState<ALMPlayerState>();

		if (!GetLevelCompleted() || !PS || PS->GetHealth() < 0)return;
		SavePixelArt();
	}
}

void ALMWhackAMoleGameMode::StartMoleMatch(const FString& Name)
{
	Super::Super::Super::StartMatch();
}

void ALMWhackAMoleGameMode::SetPlayerTurn()
{
	if (ControllerToPlay == nullptr)
	{
		ControllerToPlay = UGameplayStatics::GetPlayerController(GetWorld(), FMath::RandRange(0, 1));
	}
	if (LegoTowers.Contains(ControllerToPlay)) LegoTowers[ControllerToPlay]->PlayAnimationOnPlayerTurn(false);
	if (bRandomPlayerTurn)
	{
		ControllerToPlay = UGameplayStatics::GetPlayerController(GetWorld(), FMath::RandRange(0, 1));
	}
	else
	{
		if (ControllerToPlay == UGameplayStatics::GetPlayerController(GetWorld(), 0))
			ControllerToPlay =
				UGameplayStatics::GetPlayerController(GetWorld(), 1);
		else ControllerToPlay = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
	if (ControllerToPlay->NetPlayerIndex == 1)
	{
		SetVignetteColor(FLinearColor(0, 1.f, 0, 1.f));
		if (LuigiTurnSound) UGameplayStatics::PlaySound2D(this, LuigiTurnSound);
	}
	else
	{
		SetVignetteColor(FLinearColor(1.f, 0, 0, 1.f));
		if (MarioTurnSound) UGameplayStatics::PlaySound2D(this, MarioTurnSound);
	}
	if (LegoTowers.Contains(ControllerToPlay)) LegoTowers[ControllerToPlay]->PlayAnimationOnPlayerTurn(true);
}

void ALMWhackAMoleGameMode::HandlePlayGame()
{
	const auto LegoController = Cast<ALMWhackAMoleController>(GetWorld()->GetFirstPlayerController());
	if (ReloadedLego >= 2 && LegoController && MatchState == MatchState::InProgress)
		StartMatchCountdown(Username);
}

void ALMWhackAMoleGameMode::ReloadLego(ALMLegoTower* LegoTower)
{
	ReloadedLego++;
	if (const auto PlayerController = LegoTowers.FindKey(LegoTower))
	{
		LegoTower->SetReadyWidgetText(true, *PlayerController);
	}

	if (ReloadedLego >= 2 && MatchState == MatchState::InProgress)
		StartMatchCountdown(Username);
}

void ALMWhackAMoleGameMode::CacheLegoTowers()
{
	const TSubclassOf<AActor> LegoTowerClass = ALMLegoTower::StaticClass();

	TArray<AActor*> LegoTowerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), LegoTowerClass, LegoTowerActors);

	bool bColorMario = false;
	for (const auto LegoTowerActor : LegoTowerActors)
	{
		bColorMario = !bColorMario;
		if (auto LegoTower = Cast<ALMLegoTower>(LegoTowerActor))
		{
			LegoTower->SetTowerColor(bColorMario == true ? RED_COLOR : GREEN_COLOR);
			ALMMarioController* TowerPlayerController = bColorMario == true
				                                            ? Cast<ALMMarioController>(
					                                            UGameplayStatics::GetPlayerController(this, 0))
				                                            : Cast<ALMMarioController>(
					                                            UGameplayStatics::GetPlayerController(this, 1));
			LegoTower->ChangeFootStepsColor(TowerPlayerController->GetGameplayTags().HasTag(TAG_Gameplay_Tower_Reload));
			LegoTowers.Emplace(TowerPlayerController, LegoTower);
		}
	}
}

TArray<FPAPixelArtDataInfos>::SizeType ALMWhackAMoleGameMode::FindByTag(TArray<FPAPixelArtDataInfos> Array,
                                                                        FGameplayTag Tag)
{
	const TArray<FPAPixelArtDataInfos>::ElementType* RESTRICT Start = Array.GetData();
	for (const TArray<FPAPixelArtDataInfos>::ElementType * RESTRICT Data = Start, *RESTRICT DataEnd = Data + Array.Num()
	     ; Data != DataEnd; ++Data)
	{
		if (Data->PixelArtTag == Tag)
		{
			return static_cast<TArray<FPAPixelArtDataInfos>::SizeType>(Data - Start);
		}
	}
	return INDEX_NONE;
}

void ALMWhackAMoleGameMode::GenerateScanningTowerArray(FGameplayTag ScanShape, ALMMoleTower* MoleTower)
{
	if (!ScanMoleTowersMap.Contains(ScanShape))
	{
		TArray<ALMMoleTower*> MoleToAdd;
		MoleToAdd.Add(MoleTower);
		ScanMoleTowersMap.Emplace(ScanShape, FDebugScan{MoleToAdd});
	}
	else
	{
		ScanMoleTowersMap[ScanShape].ScanTowers.Add(MoleTower);
	}
}

void ALMWhackAMoleGameMode::SaveAndRequestNewMoleFromHit(const bool bHit)
{
	const int CurrentPixelIndex = GetCurrentPixelIndex();
	if (bHit && LevelPixelInfo.CurrentRowPixelCoordinate.IsValidIndex(CurrentPixelIndex))
	{
		PixelArtDataInfos.PixelColorMap[LevelPixelInfo.CurrentRowPixelCoordinate[CurrentPixelIndex]].PixelState =
			EPixelState::EPS_Unlocked;
		PixelArtDataInfos.PixelColorMap[LevelPixelInfo.CurrentRowPixelCoordinate[CurrentPixelIndex]].HitColor =
			GetDamageableMole()->GetTowerColor();
	}
	LevelPixelInfo.CurrentMoleRemaining--;
	OnPixelChangedDelegate.Broadcast(bHit, GetDamageableMole()->GetTowerColor());
	if (LevelPixelInfo.CurrentMoleRemaining <= 0)EndMatch();
}

int ALMWhackAMoleGameMode::GetDataLayoutOffset() const
{
	int Offset = 0;

	if (static_cast<int>(GetLevelPixelInfo().TowerHeight) > 0)Offset++;
	if (static_cast<int>(GetLevelPixelInfo().TowerAngle) >= 1)Offset++;
	if (static_cast<int>(GetLevelPixelInfo().TowerAngle) >= 2)Offset++;
	if (static_cast<int>(GetLevelPixelInfo().TowerMovement) >= 2)Offset += 2;
	if (GetLevelPixelInfo().TowerMovement == ETowerMovement::ETM_Both)Offset += 2;
	return Offset;
}
