// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/LMMoleTower.h"

#include "NiagaraFunctionLibrary.h"
#include "VectorTypes.h"
#include "Camera/CameraActor.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "HelperLibrary/LMSaveLevelSettings.h"
#include "HLSLTree/HLSLTreeTypes.h"
#include "HUD/LMMarioHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PlayerControllers/LMMarioController.h"
#include "PlayerStates/LMPlayerState.h"
#include "WorldSettings/LMMoleWorldSettings.h"

ALMMoleTower::ALMMoleTower()
{
	PrimaryActorTick.bCanEverTick = false;

	TowerStandComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerStandComponent"));
	RootComponent = TowerStandComponent;

	TowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMeshComponent"));
	TowerMeshComponent->SetupAttachment(TowerStandComponent);

	OrcTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));
}

void ALMMoleTower::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("UpdateOrcPosition"));
	FOnTimelineEvent TimelineFinished;
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));

	if (TimeLineCurve)
	{
		OrcTimeline->AddInterpFloat(TimeLineCurve, TimelineProgress);
	}
	OrcTimeline->SetTimelineFinishedFunc(TimelineFinished);


	SetTowerActivated(bIsTowerActivated);
}

UNiagaraComponent* ALMMoleTower::InitializeNiagara(UNiagaraSystem* Particules, FVector Size)
{
	if (!IsValid(this))return nullptr;
	const FVector TowerHeight = GetActorUpVector() * GetTopLocation();
	if (Particules)
	{
		const auto ParticlesComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			Particules,
			GetActorLocation() + TowerHeight,
			GetActorRotation(),
			Size
		);
		return ParticlesComponent;
	}
	return nullptr;
}

void ALMMoleTower::SetTowerBlink()
{
	const auto TowerMaterial = Cast<UMaterialInstanceDynamic>(TowerMeshComponent->GetMaterial(1));
	if (!TowerMaterial)return;
	TowerMaterial->SetScalarParameterValue("bIsBlinking", 1.f);
	TowerMeshComponent->SetMaterial(1, TowerMaterial);
	TowerMeshComponent->SetMaterial(0, TowerMaterial);
}

void ALMMoleTower::ApplyCalculatedTime(const float FailedSpawnModifier, const APlayerController* PlayerTurn)
{
	float& ExposureTime = PlayerTurn == UGameplayStatics::GetPlayerController(this, 0) ? MarioExposureTime : LuigiExposureTime;
	float& TriggerTime = PlayerTurn == UGameplayStatics::GetPlayerController(this, 0) ? MarioTriggerTime : LuigiTriggerTime;

	float Distance = FMath::Abs(ExposureTime - TriggerTime);
	float Weight = 1.f / (FMath::Loge(Distance + 1.f) + 1.f);
	CalculatedExposureTime = TriggerTime > 0 ? ExposureTime * Weight + TriggerTime * (1 - Weight) : ExposureTime;
	CalculatedExposureTime *= FailedSpawnModifier;
}

bool ALMMoleTower::SpawnMole(const float FailedSpawnModifier, const APlayerController* PlayerTurn)
{
	ApplyCalculatedTime(FailedSpawnModifier, PlayerTurn);
	SetCurveLength(CalculatedExposureTime);
	SpawnedParticules = InitializeNiagara(SpawnParticules, FVector(.1f, .1f, .1f));
	if (SpawnedParticules)
	{
		if (const auto GameMode = Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			SpawnedParticules->SetVariableFloat(FName("LoopDuration"), CalculatedExposureTime * 2.f);
		}
	}
	OrcTimeline->PlayFromStart();
	const auto World = GetWorld();
	if (!World)return false;
	SpawnedTime = World->GetTimeSeconds();
	bCanReceiveDamage = true;
	bMoleOut = true;
	if (SpawnSound) UGameplayStatics::PlaySound2D(this, SpawnSound, 1.f, 1.f, SpawnSound->Duration - 1.f);
	SetTowerBlink();
	return true;
}

void ALMMoleTower::KillMole()
{
	if (!bWaitForReloadToSpawn)OnMoleReadyToReSpawn.Broadcast();
	bCanReceiveDamage = false;
	bMoleOut = false;
	if (OrcTimeline->IsPlaying())
	{
		OrcTimeline->Stop();
	}
	if (SpawnedParticules)
	{
		SpawnedParticules->DestroyInstance();
	}
	KilledParticules = InitializeNiagara(KillParticules, FVector(.25f, .25f, .25f));
	const auto TowerMaterial = Cast<UMaterialInstanceDynamic>(TowerMeshComponent->GetMaterial(1));
	if (!TowerMaterial)return;
	TowerMaterial->SetScalarParameterValue("bIsBlinking", 0.f);
	TowerMeshComponent->SetMaterial(1, TowerMaterial);
}

void ALMMoleTower::SetCurveLength(const float& Length)
{
	if (TimeLineCurve)
	{
		const FKeyHandle Key = TimeLineCurve->FloatCurve.FindKey(TimeLineCurve->FloatCurve.GetLastKey().Time, 0.1f);
		TimeLineCurve->FloatCurve.SetKeyTime(Key, Length);
	}
}

void ALMMoleTower::ProcessPerLegoSpawnInfos(EDifficultyPattern Pattern, ULMSaveLevelSettings* LevelSave, const FGameplayTag& LevelPixelTag, FHitInfos& HitInfos, float& TriggerTime, float& ExposureTime)
{
	float XIndex = FMath::Max(HitInfos.ExposureTime.Num() * LevelSave->GetXPercentage(LevelPixelTag), HitInfos.ExposureTime.Num() * .1f);
	switch (Pattern)
	{
	case EDifficultyPattern::EDP_Best:
		ULMSaveLevelSettings::HitInfoBest(HitInfos, TriggerTime, ExposureTime);
		break;
	case EDifficultyPattern::EDP_XBest:
		ULMSaveLevelSettings::HitInfoXBest(HitInfos, TriggerTime, ExposureTime, FMath::TruncToInt(XIndex));
		break;
	case EDifficultyPattern::EDP_Average:
		ULMSaveLevelSettings::HitInfoAverage(HitInfos, TriggerTime, ExposureTime);
		break;
	case EDifficultyPattern::EDP_XWorst:
		ULMSaveLevelSettings::HitInfoXWorst(HitInfos, TriggerTime, ExposureTime, FMath::TruncToInt(XIndex));
		break;
	case EDifficultyPattern::EDP_Worst:
		ULMSaveLevelSettings::HitInfoWorst(HitInfos, TriggerTime, ExposureTime);
		break;
	default:
		break;
	}
}

void ALMMoleTower::SetTowerSpawnInfos(EDifficultyPattern Pattern, ULMSaveLevelSettings* LevelSave, const FGameplayTag& LevelPixelTag)
{
	if (!LevelSave || !LevelPixelTag.IsValid())return;
	FHitInfos MarioHitInfos;
	FHitInfos LuigiHitInfos;
	LevelSave->RequestHitInfoDatas(MarioHitInfos, LevelPixelTag, ELegoTower::ELT_Mario, TowerSeed);
	LevelSave->RequestHitInfoDatas(LuigiHitInfos, LevelPixelTag, ELegoTower::ELT_Luigi, TowerSeed);
	if (MarioHitInfos.ExposureTime.Num() > DDAThreshold)ProcessPerLegoSpawnInfos(Pattern, LevelSave, LevelPixelTag, MarioHitInfos, MarioTriggerTime, MarioExposureTime);
	if (LuigiHitInfos.ExposureTime.Num() > DDAThreshold)ProcessPerLegoSpawnInfos(Pattern, LevelSave, LevelPixelTag, LuigiHitInfos, LuigiTriggerTime, LuigiExposureTime);
}

void ALMMoleTower::UpdateOrcPosition(float Value)
{
}

void ALMMoleTower::OnTimelineFinished()
{
	if (FMath::IsNearlyEqual(TimeLineCurve->FloatCurve.GetLastKey().Time, 0.f, 0.1f))return;
	OrcTimeline->Stop();
	ALMWhackAMoleGameMode* GameMode = Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)return;

	for (auto Player : UGameplayStatics::GetGameState(GetWorld())->PlayerArray)
	// Since SplitScreen is activated (twoPlayer on same screen)
	{
		auto* LMPlayerState = Cast<ALMPlayerState>(Player);
		if (!LMPlayerState)return;
		if (!GameMode->GetLegoTowerMap().Contains(GameMode->GetPlayerTurn()))return;
		const auto* World = GetWorld();
		if (!World)return;
		const auto WorldSettings = Cast<ALMMoleWorldSettings>(World->GetWorldSettings());
		if (!WorldSettings)return;
		ULMSaveLevelSettings::FillHitInfo(
			LMPlayerState->HitMoleTelemetryInfo,
			WorldSettings->GetLevelPixelTag(),
			GameMode->GetPlayerTurn() == UGameplayStatics::GetPlayerController(World, 0)
				? ELegoTower::ELT_Mario
				: ELegoTower::ELT_Luigi,
			GetTowerSeed(),
			MarioExposureTime,
			0,
			World->GetTimeSeconds() - SpawnedTime
		);
		bCanReceiveDamage = false;
		bMoleOut = false;
		if (DamageSound) UGameplayStatics::PlaySound2D(this, DamageSound);
		LMPlayerState->RemoveHealth();
	}
	for (const auto LegoTower : GameMode->GetLegoTowerMap())
	{
		LegoTower.Value->PlayAnimationOnPlayerTurn(false);
	}
	const auto TowerMaterial = Cast<UMaterialInstanceDynamic>(TowerMeshComponent->GetMaterial(1));
	if (!TowerMaterial)return;
	TowerMaterial->SetScalarParameterValue("bIsBlinking", 0.f);
	TowerMeshComponent->SetMaterial(1, TowerMaterial);

	GameMode->SaveAndRequestNewMoleFromHit(false);
	if (!bWaitForReloadToSpawn)OnMoleReadyToReSpawn.Broadcast();
}

float ALMMoleTower::GetTopLocation() const
{
	return TowerMeshComponent->Bounds.Origin.Z + TowerMeshComponent->Bounds.BoxExtent.Z - (GetActorScale().Z * 4);
}


void ALMMoleTower::SetTowerColor(FLinearColor NewColor)
{
	const auto NewTowerColor = UMaterialInstanceDynamic::Create(TowerMeshComponent->GetMaterial(1), nullptr);

	TowerColor = NewColor;
	NewTowerColor->SetVectorParameterValue(FName("Color"), TowerColor);
	TowerMeshComponent->SetMaterial(1, NewTowerColor);
	TowerMeshComponent->SetMaterial(0, NewTowerColor);
}
