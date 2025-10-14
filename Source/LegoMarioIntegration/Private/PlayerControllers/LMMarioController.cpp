// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/LMMarioController.h"

#include "TimerManager.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameInstances/LMGameInstance.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "Gameplay/Tags/LMGameplayTags.h"
#include "HUD/LMMarioHUD.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PlayerStates/LMPlayerState.h"
#include "Socket/LMSocket.h"


ALMMarioController::ALMMarioController()
{
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_White, "08004501ffff1300");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Red, "08004501ffff1500");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Blue, "08004501ffff1700");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Yellow, "08004501ffff1800");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Black, "08004501ffff1a00");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Green, "08004501ffff2500");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Purple, "08004501ffff0c01");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Cyan, "08004501ffff4201");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Brown, "08004501ffff6a00");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_Brown, "08004501ffff3801");

	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_RotationTile, "080045011400ffff");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_StartTile, "08004501b800ffff");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_FlagTile, "08004501b700ffff");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_BlockTile, "080045012900ffff");
	GroundAndTileColorMap.Emplace(EGroudAndTileColor::EGATC_SeesawTile, "08004501ab00ffff");
}

void ALMMarioController::BeginPlay()
{
	Super::BeginPlay();
}

void ALMMarioController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(WaitForSocketBindingTimerHandle);
}

void ALMMarioController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if WITH_EDITOR // Avoid Creating console when component is generated on opening blueprint
	if (GetWorld() && !GetWorld()->IsPlayInEditor())return;
#endif

	auto GameInstance = Cast<ULMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GetWorldTimerManager().SetTimer(WaitForSocketBindingTimerHandle, [this,GameInstance]
	{
		if (GameInstance->IsReadyForBinding())
		{
			if (auto GameMode = Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(this)))
			{
				GameMode->OnAllControllerConnectedDelegate.Broadcast();
			}
			GetWorldTimerManager().ClearTimer(WaitForSocketBindingTimerHandle);
		}
	}, .5f, true);
}

void ALMMarioController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ULMGameInstance* GameInstance = Cast<ULMGameInstance>(GetGameInstance());
	if (!GameInstance)return;
	if (this == UGameplayStatics::GetPlayerController(this, 0)) GameInstance->BindMarioSocket(this);
	else GameInstance->BindLuigiSocket(this);
	if (SpawnLocation != FVector(0, 0, 0))
	{
		InPawn->SetActorLocation(SpawnLocation);
	}
}

bool ALMMarioController::HasRemainingHealth()
{
	auto GameMode = Cast<ALMTopDownGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	auto LMPlayerState = GetPlayerState<ALMPlayerState>();
	if (!GameMode || !LMPlayerState)return false;
	if (LMPlayerState->GetHealth() < 0)
	{
		return false;
	}
	return true;
}

void ALMMarioController::ClearAllHUD()
{
	MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;
	if (!MarioHUD)return;
	MarioHUD->ClearAllHUD();
}

void ALMMarioController::WinLooseUIInfo(FLinearColor TextColor, FString TextInfo)
{
	const bool bHUDValid = MarioHUD && MarioHUD->PostMatchOverlay && MarioHUD->PostMatchOverlay->TXT_GameResult &&
		MarioHUD->PostMatchOverlay->TXT_MatchSummary && MarioHUD->PostMatchOverlay->TXT_SubGameResult && GetWorld()->
		GetFirstPlayerController() == this;
	if (bHUDValid)
	{
		MarioHUD->PostMatchOverlay->TXT_GameResult->SetText(FText::FromString(TextInfo));
		MarioHUD->PostMatchOverlay->TXT_GameResult->SetColorAndOpacity(TextColor);
		MarioHUD->PostMatchOverlay->TXT_SubGameResult->SetText(FText::FromString(TextInfo));
		MarioHUD->PostMatchOverlay->TXT_SubGameResult->SetColorAndOpacity(TextColor);
		int Hits = 0;
		int Misses = 0;
		// Must Handle How many Moles have been Hit
		// Helper function that will seek for all moles whether it has been hit by Mario or Luigi
		if (!ST_String)return;
		FString HitString;
		ST_String->GetStringTable()->GetSourceString(FTextKey("HitMoles"), HitString);
		FString MissedString;
		ST_String->GetStringTable()->GetSourceString(FTextKey("MissedMoles"), MissedString);
		const FString Summary = FString::Printf(TEXT("%d %s %d %s"), Hits, *HitString, Misses, *MissedString);
		MarioHUD->PostMatchOverlay->TXT_MatchSummary->SetText(FText::FromString(Summary));
	}
}

void ALMMarioController::SetMarioDirection(FVector DirectionVector)
{
	if (!IsValid(this))return;

	MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;

	const float InitialYValue = 31;
	const bool bHUDValid = IsValid(MarioHUD) &&
		MarioHUD->DebugOverlay &&
		MarioHUD->DebugOverlay->TXT_PosX &&
		MarioHUD->DebugOverlay->TXT_PosY &&
		MarioHUD->DebugOverlay->TXT_PosZ;
	if (bHUDValid)
	{
		MarioHUD->DebugOverlay->TXT_PosX->SetText(FText::FromString(FString::SanitizeFloat(DirectionVector.X)));
		MarioHUD->DebugOverlay->TXT_PosY->SetText(FText::FromString(FString::SanitizeFloat(DirectionVector.Y)));
		MarioHUD->DebugOverlay->TXT_PosZ->SetText(FText::FromString(FString::SanitizeFloat(DirectionVector.Z)));
	}
	OnDirectionChangedDelegate.Broadcast(DirectionVector.X, DirectionVector.Z);
	/*	Must find an other way since going down like crazy with Lego will cause the Y Value to ramp up as we slow down Like So :
		[5, 29, -5] Position from : 5000
		Warning      LogTemp                   [6, 22, -3] Position from : 5000
		Warning      LogTemp                   [5, -3, 1] Position from : 5000
		Warning      LogTemp                   [-4, -63, -5] Position from : 5000
		Warning      LogTemp                   [-26, -77, 12] Position from : 5000
		Warning      LogTemp                   [-33, -48, 10] Position from : 5000
		Warning      LogTemp                   [-42, -13, 29] Position from : 5000
		Warning      LogTemp                   [-57, 120, 39] Position from : 5000
		Warning      LogTemp                   [-20, 127, 35] Position from : 5000
		Warning      LogTemp                   [-18, 123, -22] Position from : 5000
	*/
	int Difference = DirectionVector.Y - InitialYValue;
	if (DirectionVector.Y <= JumpThreshold && GameplayTags.HasTag(TAG_Gameplay_Jump))
	{
		GameplayTags.RemoveTag(TAG_Gameplay_Jump);
	}
	else if ((LastJumpHeight - InitialYValue) > JumpThreshold && !GameplayTags.HasTag(TAG_Gameplay_Jump))
	{
		GameplayTags.AddTag(TAG_Gameplay_Jump);
		return;
	}
	if (DirectionVector.Y > InitialYValue && Difference > JumpThreshold && GameplayTags.HasTag(TAG_Gameplay_Jump))
	{
		OnJumpingDelegate.Broadcast();
	}
	LastJumpHeight = DirectionVector.Y;
}

void ALMMarioController::OnMatchStateSet(const FName MatchState)
{
	if (MatchState == MatchState::InProgress)
	{
		MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;
		if (!MarioHUD)return;
		MarioHUD->AddMatchInfoOverlay();
		MarioHUD->AddAccelerometerOverlay(GetLocalPlayer()->GetLocalPlayerIndex());
	}
	if (MatchState == MatchState::WaitingPostMatch)
	{
		GetWorldTimerManager().ClearTimer(VictoryTimerHandle);
		if (ALMMarioCharacter* LegoCharacter = Cast<ALMMarioCharacter>(GetPawn()))
		{
			LegoCharacter->GetMovementComponent()->SetActive(false);
			LegoCharacter->bUseControllerRotationYaw = false;
		}
		if (this != UGameplayStatics::GetPlayerController(GetWorld(), 0))return;
		MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;
		if (!MarioHUD)return;
		MarioHUD->AddPostMatchOverlay();
		FInputModeGameAndUI InputSettings;
		InputSettings.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputSettings);
		SetShowMouseCursor(true);
	}
}

void ALMMarioController::SetMarioOutfit(const int BinaryPantsValue)
{
	if (!IsValid(this))return;
	MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;

	InitialBinaryPantsValue = BinaryPantsValue;

	const bool bHUDValid = MarioHUD &&
		MarioHUD->DebugOverlay &&
		MarioHUD->DebugOverlay->TXT_Pant;
	if (bHUDValid)
	{
		switch (BinaryPantsValue)
		{
		case 0:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_None)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_None);
			break;
		case 11:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Bee)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Bee);
			break;
		case 101:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Luigi)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Luigi);
			break;
		case 110:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Frog)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Frog);
			break;
		case 1010:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Tanooki)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Tanooki);
			break;
		case 1100:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Propeller)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Propeller);
			break;
		case 10001:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Cat)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Cat);
			break;
		case 10010:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Fire)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Fire);
			break;
		case 10100:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Penguin)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Penguin);
			break;
		case 11000:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Peach)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Peach);
			break;
		case 100001:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Mario)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Mario);
			break;
		case 100010:
			MarioHUD->DebugOverlay->TXT_Pant->SetText(
				FText::FromString(UEnum::GetValueAsString(EBinaryPantsValue::EBPV_Builder)));
			OnPantChangedDelegate.Broadcast(EBinaryPantsValue::EBPV_Builder);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Could Not Recognize Pants Binary Value"));
			break;
		}
	}
}

void ALMMarioController::ValidateGroundColor(EGroudAndTileColor LastGroundData)
{
	CheckGroundColorTimerHandle.Invalidate();
	if (LastGroundData != CurrentGroundData)
	{
		if (bSecondStepValidationRequire)VerifyGroundColor(LastGroundData);
		return;
	}
	FLinearColor GroundColor;
	switch (CurrentGroundData)
	{
	case EGroudAndTileColor::EGATC_White:
		GroundColor = WHITE_COLOR;
		break;
	case EGroudAndTileColor::EGATC_Red:
		GroundColor = RED_COLOR;
		break;
	case EGroudAndTileColor::EGATC_Blue:
		GroundColor = BLUE_COLOR;
		break;
	case EGroudAndTileColor::EGATC_Yellow:
		GroundColor = YELLOW_COLOR;
		break;
	case EGroudAndTileColor::EGATC_Green:
		GroundColor = GREEN_COLOR;
		break;
	case EGroudAndTileColor::EGATC_Purple:
		GroundColor = PURPLE_COLOR;
		break;
	case EGroudAndTileColor::EGATC_Cyan:
		GroundColor = CYAN_COLOR;
		break;
	case EGroudAndTileColor::EGATC_Brown:
		GroundColor = BROWN_COLOR;
		break;
	case EGroudAndTileColor::EGATC_BrownNouggat:
		GroundColor = BROWN_COLOR;
		break;
	default:
		GroundColor = FLinearColor::Black;
		break;
	}
	TriggerMarioGroundOrTileEvent(LastGroundData, GroundColor);
}


void ALMMarioController::VerifyGroundColor(const FString HexadecimalColorCode)
{
	if (!IsValid(this))return;
	for (auto MapValue : GroundAndTileColorMap)
	{
		if (HexadecimalColorCode == MapValue.Value)
		{
			CurrentGroundData = MapValue.Key;
			if (CheckGroundColorTimerHandle.IsValid())
			{
				bSecondStepValidationRequire = true;
				return;
			}
			bSecondStepValidationRequire = false;
			const auto LastGroundData = MapValue.Key;
			FTimerDelegate ValidateGroundColorDelegate = FTimerDelegate::CreateUObject(
				this, &ThisClass::ValidateGroundColor, LastGroundData);
			GetWorldTimerManager().SetTimer(CheckGroundColorTimerHandle, ValidateGroundColorDelegate,
			                                CheckGroundColorDelay, false);
		}
	}
}

void ALMMarioController::VerifyGroundColor(const EGroudAndTileColor LastSentGroundCode)
{
	FTimerDelegate ValidateGroundColorDelegate = FTimerDelegate::CreateUObject(
		this, &ThisClass::ValidateGroundColor, LastSentGroundCode);
	GetWorldTimerManager().SetTimer(CheckGroundColorTimerHandle, ValidateGroundColorDelegate,
	                                CheckGroundColorDelay, false);
}

void ALMMarioController::TriggerMarioGroundOrTileEvent(const EGroudAndTileColor GroundData, FLinearColor& GroundColor)
{
	MarioHUD = MarioHUD == nullptr ? Cast<ALMMarioHUD>(GetHUD()) : MarioHUD;
}
