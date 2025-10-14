// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/LMGrid.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "Gameplay/LMMoleTower.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "WorldSettings/LMMoleWorldSettings.h"


ALMGrid::ALMGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	InstancedStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMesh"));
	RootComponent = InstancedStaticMesh;
}

void ALMGrid::SpawnTowers(const FGameplayTag& ShapeTag, const EHandTrained& HandTrained, const int Offset)
{
	if (DT_GridData && DT_LayoutRightData && DT_LayoutLeftData)
	{
		auto* World = GetWorld();
		if (!World)return;
		const auto GridInfo = DT_GridData->FindGridInfo(ShapeTag);
		if (!GridInfo)return;
		auto* GameMode = Cast<ALMWhackAMoleGameMode>(World->GetAuthGameMode());
		if (!GameMode)return;
		GameMode->ClearMoleTowers();
		ULMSaveLevelSettings* LevelSave = Cast<ULMSaveLevelSettings>(UGameplayStatics::LoadGameFromSlot(GameMode->GetUsername(), 0));
		if (!LevelSave)return;
		ALMMoleWorldSettings* WorldSettings = Cast<ALMMoleWorldSettings>(UGameplayStatics::GetActorOfClass(World, ALMMoleWorldSettings::StaticClass()));
		if (!WorldSettings)return;
		GameMode->SetDifficultyPattern(LevelSave->GetDifficultyLevel(WorldSettings->GetLevelPixelTag()));

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if (GameMode->DoScanPlayer())
		{
			for (int i = 0; i < InstancedStaticMesh->PerInstanceSMData.Num(); i++)
			{
				ALMMoleTower* SpawnedTower = World->SpawnActor<ALMMoleTower>(GridInfo->Actor,
				                                                             FVector(
					                                                             InstancedStaticMesh->PerInstanceSMData[
						                                                             i].Transform.
						                                                                GetOrigin()) + GetActorLocation(),
				                                                             FRotator(),
				                                                             SpawnParameters);
				if (!SpawnedTower)continue;
				if (i == 0 || i == GridTileCount.Y - 1 || i == GridTileCount.Y * (GridTileCount.X - 1) || i ==
					InstancedStaticMesh->PerInstanceSMData.Num() - 1)
				{
					GameMode->GenerateScanningTowerArray(TAG_Scan_Corner, SpawnedTower);
				}
				if (i < GridTileCount.Y || i >= GridTileCount.Y * (GridTileCount.X - 1))
				{
					GameMode->GenerateScanningTowerArray(TAG_Scan_BottomTop, SpawnedTower);
				}
				if (i == 0 || i % static_cast<int>(GridTileCount.Y) == 0 || (i + 1) % static_cast<int>(GridTileCount.Y)
					== 0)
				{
					GameMode->GenerateScanningTowerArray(TAG_Scan_LeftRight, SpawnedTower);
				}
				GameMode->GenerateScanningTowerArray(TAG_Scan_Line, SpawnedTower);
			}
		}
		else
		{
			TArray<FLayoutActivatedTowers*> TableRows;
			GameMode->GetLevelPixelInfo().HandTrained == EHandTrained::EHT_Right ? DT_LayoutRightData->GetAllRows<FLayoutActivatedTowers>(FString(), TableRows) : DT_LayoutLeftData->GetAllRows<FLayoutActivatedTowers>(FString(), TableRows);
			TArray<FString> LevelLayout = ULMLayoutTowers::FindLayoutInfo(WorldSettings->GetLevelPixelTag(), TableRows, Offset);
			TArray<AActor*> MoleTowerActors;
			TArray<uint8> PositionArray;
			for (auto Layout : LevelLayout)
			{
				int Position = FCString::Atoi(*Layout.Mid(0, 2));
				FString Seed = *Layout.Mid(2, 3);
				int Height = FCString::Atoi(*Layout.Mid(3, 1));

				ALMMoleTower* SpawnedTower = SpawnTowerAtLocation(Position, *GridInfo, SpawnParameters);
				if (!SpawnedTower)continue;
				auto TowerMeshComponent = SpawnedTower->GetTowerMeshComponent();
				auto TowerStandComponent = SpawnedTower->GetTowerStandComponent();
				if (!TowerMeshComponent || !TowerStandComponent || !TowerStandComponent->GetStaticMesh())return;
				const auto Socket = TowerStandComponent->GetStaticMesh()->FindSocket(FName(Seed));
				if (Socket)
					TowerMeshComponent->AttachToComponent(TowerStandComponent,
					                                      FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					                                      Socket->SocketName);
				SetTowerHeight(Height, SpawnedTower);
				SpawnedTower->SetTowerActivated(true);
				SpawnedTower->SetWaitForReloadToSpawn(GameMode->GetWaitForReloadToSpawn());
				SpawnedTower->SetTowerSeed(Layout);
				SpawnedTower->SetTowerSpawnInfos(GameMode->GetDifficultyPattern(), LevelSave, WorldSettings->GetLevelPixelTag());
				if (FCString::Atoi(*Layout.Mid(4, 1)) == 2 && HandTrained == EHandTrained::EHT_Left)
				{
					SpawnedTower->AddActorWorldRotation(FRotator(0.f, 180.f, 0.f));
				}
				MoleTowerActors.Add(SpawnedTower);
				PositionArray.Add(Position);
			}
			if (!MoleTowerActors.IsEmpty())GameMode->InitializeMoleTowers(MoleTowerActors);
			else
			{
				FString Layout = "00000";
				int Position = FCString::Atoi(*Layout.Mid(0, 2));
				FString Seed = *Layout.Mid(2, 3);
				int Height = FCString::Atoi(*Layout.Mid(3, 1));

				ALMMoleTower* SpawnedTower = SpawnTowerAtLocation(Position, *GridInfo, SpawnParameters);
				if (!SpawnedTower)return;;
				auto TowerMeshComponent = SpawnedTower->GetTowerMeshComponent();
				auto TowerStandComponent = SpawnedTower->GetTowerStandComponent();
				if (!TowerMeshComponent || !TowerStandComponent || !TowerStandComponent->GetStaticMesh())return;
				const auto Socket = TowerStandComponent->GetStaticMesh()->FindSocket(FName(Seed));
				if (Socket)
					TowerMeshComponent->AttachToComponent(TowerStandComponent,
					                                      FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					                                      Socket->SocketName);
				SetTowerHeight(Height, SpawnedTower);
				SpawnedTower->SetTowerActivated(true);
				SpawnedTower->SetWaitForReloadToSpawn(GameMode->GetWaitForReloadToSpawn());
				SpawnedTower->SetTowerSeed(Layout);
				SpawnedTower->SetTowerSpawnInfos(GameMode->GetDifficultyPattern(), LevelSave, WorldSettings->GetLevelPixelTag());
				if (FCString::Atoi(*Layout.Mid(4, 1)) == 2 && HandTrained == EHandTrained::EHT_Left)
				{
					SpawnedTower->AddActorWorldRotation(FRotator(0.f, 180.f, 0.f));
				}
				MoleTowerActors.Add(SpawnedTower);
				PositionArray.Add(Position);
				GameMode->InitializeMoleTowers(MoleTowerActors);
			}
			for (int i = 0; i < InstancedStaticMesh->PerInstanceSMData.Num(); i++)
			{
				if (!PositionArray.Contains(i)) SpawnTowerAtLocation(i, *GridInfo, SpawnParameters);
			}
		}
	}
}

void ALMGrid::BeginPlay()
{
	Super::BeginPlay();
}

void ALMGrid::SpawnGrid(const FVector& Center, const FVector TileSize, const FVector2D& TileCount,
                        const FGameplayTag TileShape)
{
	if (!InstancedStaticMesh)return;
	InstancedStaticMesh->ClearInstances();
	if (DT_GridData)
	{
		auto GridInfo = DT_GridData->FindGridInfo(TileShape);
		if (!GridInfo)return;
		InstancedStaticMesh->SetStaticMesh(GridInfo->FlatStaticMesh);
		InstancedStaticMesh->SetMaterial(0, GridInfo->FlatMaterial);
		float SquareSize = FMath::Max(TileCount.X, TileCount.Y);
		FVector GridBottomLeftCorner = Center - (FVector(SquareSize, SquareSize, 0) / 2) * TileSize * GridInfo->
			MeshSize;
		for (int X = 0; X < TileCount.X; X++)
		{
			for (int Y = 0; Y < TileCount.Y; Y++)
			{
				FTransform Transform;
				FVector TransformScale = TileSize;
				FVector TransformLocation = FVector(GridBottomLeftCorner.X * X, GridBottomLeftCorner.Y * Y, 0);
				Transform.SetScale3D(TransformScale);
				Transform.SetLocation(TransformLocation);
				InstancedStaticMesh->AddInstance(Transform);
				UE_LOG(LogTemp, Warning, TEXT("Transform: %s"), *Transform.GetLocation().ToString());
			}
		}
	}
}

ALMMoleTower* ALMGrid::SpawnTowerAtLocation(const int Position, const FGridInfo& GridInfo,
                                            const FActorSpawnParameters& SpawnParameters)
{
	const auto World = GetWorld();
	if (!World)return nullptr;
	if (InstancedStaticMesh->PerInstanceSMData.IsValidIndex(Position))
	{
		return World->SpawnActor<ALMMoleTower>(GridInfo.Actor,
		                                       FVector(
			                                       InstancedStaticMesh->PerInstanceSMData[
				                                       Position].Transform.
				                                                 GetOrigin()) + GetActorLocation(),
		                                       FRotator(),
		                                       SpawnParameters);
	}
	return nullptr;
}

void ALMGrid::SetTowerHeight(int Height, ALMMoleTower* SpawnedTower)
{
	FVector SpawnedTowerScale = SpawnedTower->GetActorScale3D();
	int TowerHeight = SpawnedTowerScale.Z;
	switch (static_cast<ETowerHeight>(Height))
	{
	case ETowerHeight::ETH_10:
		TowerHeight = 40;
		break;
	case ETowerHeight::ETH_20:
		TowerHeight = 80;
		break;
	default: ;
	}
	SpawnedTowerScale.Z = TowerHeight;
	SpawnedTower->SetActorScale3D(SpawnedTowerScale);
}