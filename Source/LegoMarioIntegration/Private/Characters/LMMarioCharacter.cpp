// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LMMarioCharacter.h"

#include "TimerManager.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "HUD/LMMarioHUD.h"
#include "Inputs/LMTaggedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PlayerControllers/LMMarioController.h"


ALMMarioCharacter::ALMMarioCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_None, FOutfitMaterialColor({FVector::Zero(), FVector::Zero()}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Bee, FOutfitMaterialColor({YELLOW_OUTFIT,BROWN_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Luigi, FOutfitMaterialColor({GREEN_OUTFIT,BLUE_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Frog, FOutfitMaterialColor({GREEN_OUTFIT,GREEN_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Tanooki, FOutfitMaterialColor({BROWN_OUTFIT,BROWN_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Propeller, FOutfitMaterialColor({RED_OUTFIT,RED_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Cat, FOutfitMaterialColor({YELLOW_OUTFIT,BEIGE_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Fire, FOutfitMaterialColor({WHITE_OUTFIT,RED_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Penguin, FOutfitMaterialColor({RED_OUTFIT,BLUE_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Peach, FOutfitMaterialColor({PINK_OUTFIT,DARKPINK_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Mario, FOutfitMaterialColor({BLUE_OUTFIT,RED_OUTFIT}));
	WearedOutfitMap.Emplace(EBinaryPantsValue::EBPV_Builder, FOutfitMaterialColor({RED_OUTFIT,YELLOW_OUTFIT}));
}

void ALMMarioCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if ((MarioController = MarioController == nullptr ? Cast<ALMMarioController>(NewController) : MarioController))
	{
		MarioController->OnPantChangedDelegate.AddDynamic(this, &ThisClass::ChangePant);
	}
}

void ALMMarioCharacter::BeginPlay()
{
	Super::BeginPlay();

	PantsSleevesHandMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	ShirtMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(1), this);
	//Set Default Mario Skin
	if (PantsSleevesHandMaterial)
	{
		PantsSleevesHandMaterial->SetVectorParameterValue("Tint", FVector(0, 0, 1));
		GetMesh()->SetMaterial(0, PantsSleevesHandMaterial);
	}

	if (ShirtMaterial)
	{
		ShirtMaterial->SetVectorParameterValue("Tint", FVector(1, 0, 0));
		GetMesh()->SetMaterial(1, ShirtMaterial);
	}

	if (Cast<ALMTopDownGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		SetActorScale3D(GetActorScale() * 3);
	}
}

void ALMMarioCharacter::ChangePant(EBinaryPantsValue PantsValue)
{
	ShirtMaterial->SetVectorParameterValue("Tint", WearedOutfitMap[PantsValue].ShirtColor);
	PantsSleevesHandMaterial->SetVectorParameterValue("Tint", WearedOutfitMap[PantsValue].PantsSleevesHeadColor);
	GetMesh()->SetMaterial(0, PantsSleevesHandMaterial);
	GetMesh()->SetMaterial(1, ShirtMaterial);
	if (!GetMesh()->IsVisible())
	{
		GetMesh()->SetVisibility(true);
	}
}

void ALMMarioCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALMMarioCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* TaggedInputComponent = Cast<ULMTaggedInputComponent>(PlayerInputComponent);
	if (!ensureAlwaysMsgf(TaggedInputComponent, TEXT("Verify input in project setting and look for InputConfig within Character BP")))return;

	TaggedInputComponent->BindActionsByTag(InputConfig, TAG_Input_Pause, ETriggerEvent::Triggered, this, &ALMMarioCharacter::InputPause);
}

void ALMMarioCharacter::InputPause(const FInputActionValue& InputActionValue)
{
	if (MarioController)
	{
		if (const auto MarioHUD = Cast<ALMMarioHUD>(MarioController->GetHUD()))
		{
			if (!MarioHUD->PauseOptionsOverlay || !MarioHUD->PauseOptionsOverlay->IsInViewport() || !MarioHUD->PauseOptionsOverlay->IsVisible())
			{
				MarioHUD->AddPauseOptionsOverlay();
				MarioController->SetShowMouseCursor(true);
				FInputModeGameAndUI InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				MarioController->SetInputMode(InputMode);
				MarioController->SetPause(true);
			}
			else
			{
				MarioHUD->HidePauseOptionsOverlay();
				MarioController->SetPause(false);
			}
		}
	}
}
