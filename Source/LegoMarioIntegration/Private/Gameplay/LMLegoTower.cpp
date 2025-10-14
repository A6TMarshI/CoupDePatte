// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/LMLegoTower.h"

#include "Components/TextBlock.h"
#include "HUD/LMLegoReadyWidget.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Kismet/GameplayStatics.h"
#include "LegoMarioIntegration/LegoMarioIntegration.h"
#include "Materials/MaterialInstanceDynamic.h"


ALMLegoTower::ALMLegoTower()
{
	PrimaryActorTick.bCanEverTick = false;

	TowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMeshComponent"));
	RootComponent = TowerMeshComponent;

	FootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FeetMeshComponent"));
	FootMeshComponent->SetupAttachment(TowerMeshComponent);

	PlayerTurnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));

	ReadyWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ReadyWidgetComponent"));
	ReadyWidgetComponent->SetupAttachment(TowerMeshComponent);
	ReadyWidgetComponent->SetVisibility(false);
}

void ALMLegoTower::SetTowerColor(FLinearColor NewColor)
{
	const auto NewTowerColor = UMaterialInstanceDynamic::Create(TowerMeshComponent->GetMaterial(1), nullptr);

	TowerColor = NewColor;
	NewTowerColor->SetVectorParameterValue(FName("Color"), TowerColor);
	TowerMeshComponent->SetMaterial(1, NewTowerColor);
}

void ALMLegoTower::ChangeFootStepsColor(const bool bCanAttack)
{
	UMaterialInstanceDynamic* NewGroundMaterial = Cast<UMaterialInstanceDynamic>(FootMeshComponent->GetMaterial(0));
	NewGroundMaterial = NewGroundMaterial == nullptr ? UMaterialInstanceDynamic::Create(FootMeshComponent->GetMaterial(0), nullptr) : NewGroundMaterial;
	NewGroundMaterial->SetScalarParameterValue(MATERIAL_FOOT_COLOR, bCanAttack);
	FootMeshComponent->SetMaterial(0, NewGroundMaterial);
}

void ALMLegoTower::PlayAnimationOnPlayerTurn(const bool bPlay)
{
	const auto TowerMaterial = Cast<UMaterialInstanceDynamic>(TowerMeshComponent->GetMaterial(1));
	if (!TowerMaterial)return;
	if (bPlay)
	{
		TowerMaterial->SetScalarParameterValue("bIsBlinking", 1.f);
		TowerMeshComponent->SetMaterial(1, TowerMaterial);
		PlayerTurnTimeline->PlayFromStart();
	}
	else
	{
		TowerMaterial->SetScalarParameterValue("bIsBlinking", 0.f);
		TowerMeshComponent->SetMaterial(1, TowerMaterial);
		FootMeshComponent->SetRelativeLocation(FVector(0, 0,FOOT_HEIGHT));
		PlayerTurnTimeline->Stop();
	}
}

void ALMLegoTower::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindDynamic(this, &ThisClass::UpdateFootShakingPosition);

	if (TimeLineCurve)
	{
		PlayerTurnTimeline->AddInterpFloat(TimeLineCurve, TimelineProgress);
		PlayerTurnTimeline->SetLooping(true);
	}
}

void ALMLegoTower::UpdateFootShakingPosition(float Value)
{
	if (FootMeshComponent)
	{
		FVector NewLocation = FVector(FootMeshComponent->GetComponentLocation().X + Value,
		                              FootMeshComponent->GetComponentLocation().Y - Value, FootMeshComponent->GetComponentLocation().Z);
		FootMeshComponent->SetWorldLocation(NewLocation);
	}
}

void ALMLegoTower::SetReadyWidgetText(bool bReady, const APlayerController* PlayerController) const
{
	const auto LegoController = Cast<ALMMarioController>(PlayerController);
	if (!LegoController || !LegoController->ST_String)return;
	if (const auto Widget = Cast<ULMLegoReadyWidget>(ReadyWidgetComponent->GetWidget()))
	{
		FTextKey Key = LegoController == UGameplayStatics::GetPlayerController(GetWorld(), 0) ? FTextKey("Mario") : FTextKey("Luigi");
		const auto String = bReady
			                    ? LegoController->ST_String->GetStringTable().Get().FindEntry(
				                    LegoController == UGameplayStatics::GetPlayerController(GetWorld(), 0) ? FTextKey("MarioReady") : FTextKey("LuigiReady"))
			                    : LegoController->ST_String->GetStringTable().Get().FindEntry(
				                    LegoController == UGameplayStatics::GetPlayerController(GetWorld(), 0) ? FTextKey("MarioWaitingReady") : FTextKey("LuigiWaitingReady"));
		Widget->TXT_LegoState->SetText(FText::FromString(String.Get()->GetSourceString()));
	}
}
