// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Inputs/LMInputConfig.h"
#include "LMMarioCharacter.generated.h"

#define YELLOW_OUTFIT FVector(1,.8f,0)
#define BROWN_OUTFIT FVector(.647f,.165f,.165f)
#define GREEN_OUTFIT FVector(0,1,0)
#define RED_OUTFIT FVector(1,0,0)
#define BLUE_OUTFIT FVector(0,0,1)
#define BEIGE_OUTFIT FVector(.4,.3,.1)
#define WHITE_OUTFIT FVector(1,1,1)
#define PINK_OUTFIT FVector(1,.3f,1)
#define DARKPINK_OUTFIT FVector(1,0,1)

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	EMD_NONE UMETA(DisplayName = "None"),

	EMD_UP UMETA(DisplayName = "Up"),
	EMD_DOWN UMETA(DisplayName = "Down")
};

UENUM(BlueprintType)
enum class EBinaryPantsValue : uint8
{
	EBPV_None UMETA(DisplayName = "None"),

	EBPV_Bee UMETA(DisplayName = "Bee"),
	EBPV_Luigi UMETA(DisplayName = "Luigi"),
	EBPV_Frog UMETA(DisplayName = "Frog"),
	EBPV_Tanooki UMETA(DisplayName = "Tanooki"),
	EBPV_Propeller UMETA(DisplayName = "Propeller"),
	EBPV_Cat UMETA(DisplayName = "Cat"),
	EBPV_Fire UMETA(DisplayName = "Fire"),
	EBPV_Penguin UMETA(DisplayName = "Penguin"),
	EBPV_Peach UMETA(DisplayName = "Peach"),
	EBPV_Mario UMETA(DisplayName = "Mario"),
	EBPV_Builder UMETA(DisplayName = "Builder"),
};

USTRUCT()
struct FOutfitMaterialColor
{
	GENERATED_BODY()

	FVector PantsSleevesHeadColor;
	FVector ShirtColor;
};

class ALMMarioController;

UCLASS()
class LEGOMARIOINTEGRATION_API ALMMarioCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALMMarioCharacter();

	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// This is where mesh switch would happen, if custom mesh were available. Within our restricted assets, updating defaults Manny Mesh material layer was implemented.
	/// @param PantsValue Pant value received by controller's binary event from socket communication
	UFUNCTION()
	void ChangePant(EBinaryPantsValue PantsValue);

	UPROPERTY(EditDefaultsOnly, Category="InputProperties")
	const ULMInputConfig* InputConfig;

	void InputPause(const FInputActionValue& InputActionValue);

private:
	UPROPERTY(VisibleAnywhere)
	class ALMMarioController* MarioController;

	UPROPERTY(VisibleAnywhere, Category="MeshProperties")
	UMaterialInstanceDynamic* PantsSleevesHandMaterial;
	UPROPERTY(VisibleAnywhere, Category="MeshProperties")
	UMaterialInstanceDynamic* ShirtMaterial;

	UPROPERTY(VisibleAnywhere, Category="MeshProperties")
	TMap<EBinaryPantsValue, FOutfitMaterialColor> WearedOutfitMap;
};
