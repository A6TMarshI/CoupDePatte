// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/LMWhackAMoleController.h"

#include "Camera/CameraActor.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "Gameplay/LMGrid.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "HelperLibrary/LMSaveLevelSettings.h"
#include "HUD/LMMarioHUD.h"
#include "HUD/PAPixelArtWidget.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Kismet/GameplayStatics.h"
#include "LegoMarioIntegration/LegoMarioIntegration.h"
#include "PlayerStates/LMPlayerState.h"
#include "WorldSettings/LMMoleWorldSettings.h"

void ALMWhackAMoleController::SetMarioDirection(FVector DirectionVector)
{
	Super::SetMarioDirection(DirectionVector);
}

void ALMWhackAMoleController::OnMatchStateSet(const FName MatchState)
{
	Super::OnMatchStateSet(MatchState);

	if (MatchState == MatchState::WaitingToStart)
	{
		MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;
		if (!MarioHUD)return;
		if (UGameplayStatics::GetPlayerController(this, 0) != this)return;
		MarioHUD->AddMatchInfoOverlay();
		MarioHUD->AddPixelArtOverlay();
		if (const auto GameMode = Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			MarioHUD->PixelArtOverlay->SetPixelArtTag(GameMode->GetPixelArtDataInfos());
			MarioHUD->SetLayoutByTrainedHand(GameMode->GetLevelPixelInfo().HandTrained);
		}
		MarioHUD->SetPixelRowVisibility(ESlateVisibility::Hidden);
		MarioHUD->SetAccelerometerOverlayVisibility(ESlateVisibility::Hidden);
	}
	if (MatchState == MatchState::InProgress)
	{
		MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;
		if (!MarioHUD->MatchOverlay)MarioHUD->AddMatchInfoOverlay();
		if (UGameplayStatics::GetPlayerController(GetWorld(), 0) != this)return;
		if (!MarioHUD)return;
		MarioHUD->SetAccelerometerOverlayVisibility(ESlateVisibility::Hidden);
	}
	if (MatchState == MatchState::PlayGame)
	{
		const auto World = GetWorld();
		if (!World)return;
		const auto Gamemode = Cast<ALMWhackAMoleGameMode>(World->GetAuthGameMode());
		if (!Gamemode) return;
		const auto LMPlayerState = GetPlayerState<ALMPlayerState>();
		if (LMPlayerState)LMPlayerState->SetHealth(Gamemode->GetHealth());
		
		MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;
		if (UGameplayStatics::GetPlayerController(GetWorld(), 0) != this)return;
		if (!MarioHUD)return;
		//StartTimer();
		MarioHUD->SetMatchInfoOverlayVisibility(ESlateVisibility::Visible);
		MarioHUD->SetPixelRowVisibility(ESlateVisibility::Visible);
	}
	if (MatchState == MatchState::WaitingPostMatch)
	{
		if (!ST_String)return;
		FString PostMatchString;
		const auto GameMode = Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(this));
		if (!GameMode || !GameMode->GetLevelCompleted())return;
		if (HasRemainingHealth())
		{
			if (VictorySound && UGameplayStatics::GetPlayerController(GetWorld(), 0) == this)
				UGameplayStatics::PlaySound2D(this, VictorySound);
			ST_String->GetStringTable()->GetSourceString(FTextKey("Win"), PostMatchString);
			const FString Summary = FString::Printf(TEXT("%s"), *PostMatchString);
			WinLooseUIInfo(FLinearColor::Green, Summary);
		}
		else
		{
			if (DefeatSound && UGameplayStatics::GetPlayerController(GetWorld(), 0) == this)
				UGameplayStatics::PlaySound2D(this, DefeatSound);
			ST_String->GetStringTable()->GetSourceString(FTextKey("Loose"), PostMatchString);
			const FString Summary = FString::Printf(TEXT("%s"), *PostMatchString);
			WinLooseUIInfo(FLinearColor::Red, Summary);
		}
	}
}

void ALMWhackAMoleController::SetCameraByTrainedHand()
{
	if (ACameraActor* Camera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass())))
	{
		if (ALMWhackAMoleGameMode* GameMode = Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			FVector Location = FVector(2400.f, 0.f, 2100.f);
			switch (GameMode->GetLevelPixelInfo().HandTrained)
			{
			case EHandTrained::EHT_Right:
				Location += FVector(0.f, -400.f, 0.f);
				break;
			default:
				Location += FVector(0.f, 400.f, 0.f);
				break;
			}
			Camera->SetActorLocation(Location);
		}
	}
}

void ALMWhackAMoleController::BeginPlay()
{
	Super::BeginPlay();

	bAutoManageActiveCameraTarget = false;
}

void ALMWhackAMoleController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALMWhackAMoleController::TriggerMarioGroundOrTileEvent(const EGroudAndTileColor GroundData,
                                                            FLinearColor& GroundColor)
{
	Super::TriggerMarioGroundOrTileEvent(GroundData, GroundColor);

	if (!UGameplayStatics::GetGameMode(GetWorld())->HasMatchStarted())return;

	if (auto WhackAMoleGameMode = Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (!WhackAMoleGameMode->GetLegoTowerMap().Contains(this))return;
		ALMLegoTower* LegoTower = WhackAMoleGameMode->GetLegoTowerMap()[this];
		ReloadLegoAttack(GroundColor, LegoTower, WhackAMoleGameMode);
		if (WhackAMoleGameMode->GetPlayerTurn() != this)return;
		if (!WhackAMoleGameMode->GetDamageableMole())return;
		if (WhackAMoleGameMode->GetDamageableMole()->GetTowerColor() == GroundColor && WhackAMoleGameMode->
		                                                                               GetDamageableMole()->CanReceiveDamage() && GameplayTags.HasTag(TAG_Gameplay_Tower_Reload))
		{
			if (FireBallSound) UGameplayStatics::PlaySound2D(this, FireBallSound);
			GameplayTags.RemoveTag(TAG_Gameplay_Tower_Reload);
			LegoTower->SetReadyWidgetText(false, *WhackAMoleGameMode->GetLegoTowerMap().FindKey(LegoTower));
			LegoTower->ChangeFootStepsColor(GameplayTags.HasTag(TAG_Gameplay_Tower_Reload));
			WhackAMoleGameMode->GetDamageableMole()->KillMole();
			LegoTower->PlayAnimationOnPlayerTurn(false);
			auto* World = GetWorld();
			if (!World || !World->GetGameState())return;
			const auto WorldSettings = Cast<ALMMoleWorldSettings>(World->GetWorldSettings());
			if (!WorldSettings)return;
			for (auto PS : World->GetGameState()->PlayerArray) // Since it is SplitScreen
			{
				auto* LMPlayerState = Cast<ALMPlayerState>(PS);
				if (!LMPlayerState)return;
				float HitTime = World->GetTimeSeconds() - WhackAMoleGameMode->GetDamageableMole()->GetSpawnedTime();
				float Exposure = WhackAMoleGameMode->GetDamageableMole()->GetExposureTime();
				float NextSpawnTime = WhackAMoleGameMode->GetNextSpawnTime();
				if (FMath::IsNearlyEqual(Exposure, 0.f, 0.1f) || FMath::IsNearlyEqual(NextSpawnTime, 0.f, 0.1f))
				{
					Exposure = HitTime;
					NextSpawnTime = HitTime;
				}

				ULMSaveLevelSettings::FillHitInfo(
					LMPlayerState->HitMoleTelemetryInfo,
					WorldSettings->GetLevelPixelTag(),
					WhackAMoleGameMode->GetPlayerTurn() == UGameplayStatics::GetPlayerController(World, 0)
						? ELegoTower::ELT_Mario
						: ELegoTower::ELT_Luigi,
					WhackAMoleGameMode->GetDamageableMole()->GetTowerSeed(),
					Exposure,
					1.f,
					HitTime
				);
			}
			WhackAMoleGameMode->SaveAndRequestNewMoleFromHit(true);
		}
	}
}

void ALMWhackAMoleController::ReloadLegoAttack(const FLinearColor& GroundColor, ALMLegoTower* LegoTower,
                                               const ALMWhackAMoleGameMode* GameMode)
{
	if (GroundColor == LegoTower->GetTowerColor())
	{
		if (!GameplayTags.HasTag(TAG_Gameplay_Tower_Reload))
		{
			GameMode->OnReloadedLegoDelegate.Broadcast(LegoTower);
			GameplayTags.AddTag(TAG_Gameplay_Tower_Reload);
			LegoTower->ChangeFootStepsColor(true);
			if (ReloadSound) UGameplayStatics::PlaySound2D(this, ReloadSound);
		}
		if (GameMode->GetDamageableMole() && GameMode->GetWaitForReloadToSpawn() && !GameMode->GetDamageableMole()->IsMoleOut() && GameMode->GetPlayerTurn() == this)
		{
			GameMode->GetDamageableMole()->OnMoleReadyToReSpawn.Broadcast();
		}
	}
}
