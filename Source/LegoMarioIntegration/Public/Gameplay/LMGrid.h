// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LMMoleTower.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Datas/LMGridConfig.h"
#include "Datas/LMLayoutTowers.h"
#include "GameFramework/Actor.h"
#include "LMGrid.generated.h"

UCLASS()
class LEGOMARIOINTEGRATION_API ALMGrid : public AActor
{
	GENERATED_BODY()

public:
	ALMGrid();

	/// Spawn Towers over the InstacedMeshs based on HandTrained and Data Offset following the corresponding Seeds.
	/// @param ShapeTag Grid Data Infos tag (only Square Available)
	/// @param HandTrained Mirror layout placement based on HandTrained Enum
	/// @param Offset Offset to gather the correct Seeds Layout (62 Level with 8 different offset <see cref="GetDataLayoutOffset\">)
	UFUNCTION(BlueprintCallable)
	void SpawnTowers(const FGameplayTag& ShapeTag, const EHandTrained& HandTrained, const int Offset);

protected:
	virtual void BeginPlay() override;

	/// Spawn InstancedMesh in a grid format, computing size matching mostly the created Physical layout.
	/// @param Center Center of Grid System
	/// @param TileSize Size of the Square spawned at location
	/// @param TileCount Number of tiles to spawn
	/// @param TileShape Shape of tiles (Only Square available)
	UFUNCTION(BlueprintCallable)
	void SpawnGrid(const FVector& Center, const FVector TileSize, const FVector2D& TileCount,
	               const FGameplayTag TileShape);
	/// Spawn the Tower on the instancedMesh location based on the given Seed Location
	/// @param Position InstancedMesh Grid Position
	/// @param GridInfo Grid Datas
	/// @param SpawnParameters Spawn Parameters
	/// @return Spawned Tower Reference
	ALMMoleTower* SpawnTowerAtLocation(const int Position, const FGridInfo& GridInfo,
	                                   const FActorSpawnParameters& SpawnParameters);
	/// Set the SpawnedTower height based on the given Seed Height Value
	/// @param Height Enum value for SpawnedTower <see cref="ETowerHeight\">
	/// @param SpawnedTower Tower that height will be adjusted
	void SetTowerHeight(int Height, ALMMoleTower* SpawnedTower);

private:
	UPROPERTY(VisibleAnywhere, Category="GridProperties")
	UInstancedStaticMeshComponent* InstancedStaticMesh;

	UPROPERTY(EditAnywhere, Category="GridProperties")
	ULMGridConfig* DT_GridData;

	UPROPERTY(EditAnywhere, Category="GridProperties")
	UDataTable* DT_LayoutRightData;

	UPROPERTY(EditAnywhere, Category="GridProperties")
	UDataTable* DT_LayoutLeftData;

	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly, meta=(AllowPrivateAccess))
	FVector GridCenter;

	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly, meta=(AllowPrivateAccess))
	FVector GridTileSize;

	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadOnly, meta=(AllowPrivateAccess))
	FVector2D GridTileCount;

	UPROPERTY(EditAnywhere, Category="GridProperties", BlueprintReadWrite, meta=(AllowPrivateAccess, Categories="Data"))
	FGameplayTag GridTileShape;

public:
	FORCEINLINE FVector2D GetTileCount() const { return GridTileCount; }
	FORCEINLINE TArray<FInstancedStaticMeshInstanceData> GetTowersInstance() const
	{
		return InstancedStaticMesh->PerInstanceSMData;
	}
};
