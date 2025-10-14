// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LMMarioHUD.h"

#include "Components/CanvasPanelSlot.h"
#include "GameModes/LMMarioGameMode.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "HUD/LMPostMatchWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/LMMarioController.h"

void ALMMarioHUD::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* MarioController = GetOwningPlayerController())
	{
		if (MarioController == nullptr)return;
		if (AccelerometerClass == nullptr)return;
		DebugOverlay = DebugOverlay == nullptr
			               ? CreateWidget<ULMAccelerometerWidget>(MarioController, AccelerometerClass)
			               : DebugOverlay;
		if (!MatchOverlayClass)return;
		MatchOverlay = MatchOverlay == nullptr
			               ? CreateWidget<ULMMatchInfoWidget>(MarioController, MatchOverlayClass)
			               : MatchOverlay;
		if (!MatchStartingCountdownClass)return;
		MatchStartingOverlay = MatchStartingOverlay == nullptr
			                       ? CreateWidget<ULMStartmatchCountdownWidget>(
				                       MarioController, MatchStartingCountdownClass)
			                       : MatchStartingOverlay;
	}
}

void ALMMarioHUD::AddAccelerometerOverlay(int PlayerIndex)
{
	if (APlayerController* MarioController = GetOwningPlayerController())
	{
		DebugOverlay = DebugOverlay == nullptr
			               ? CreateWidget<ULMAccelerometerWidget>(MarioController, AccelerometerClass)
			               : DebugOverlay;
	}
	if (!DebugOverlay)return;

	DebugOverlay->AddToPlayerScreen(PlayerIndex);

	if (PlayerIndex == 0)
	{
		DebugOverlay->TXT_Info->SetText(FText::FromString("Mario's Info :"));
	}
	else
	{
		DebugOverlay->TXT_Info->SetText(FText::FromString("Luigi's Info :"));
	}
}

void ALMMarioHUD::AddMatchInfoOverlay()
{
	if (APlayerController* MarioController = GetOwningPlayerController())
	{
		MatchOverlay = MatchOverlay == nullptr
			               ? CreateWidget<ULMMatchInfoWidget>(MarioController, MatchOverlayClass)
			               : MatchOverlay;
	}
	if (!MatchOverlay)return;
	MatchOverlay->AddToViewport();
}

void ALMMarioHUD::AddPixelArtOverlay()
{
	if (APlayerController* MarioController = GetOwningPlayerController())
	{
		PixelArtOverlay = PixelArtOverlay == nullptr
			                  ? CreateWidget<UPAPixelArtWidget>(MarioController, PixelArtClass)
			                  : PixelArtOverlay;
	}
	if (!PixelArtOverlay)return;
	PixelArtOverlay->AddToViewport();
}

void ALMMarioHUD::AddMatchStartingCountdown()
{
	if (APlayerController* MarioController = GetOwningPlayerController())
	{
		MatchStartingOverlay = MatchStartingOverlay == nullptr
			                       ? CreateWidget<ULMStartmatchCountdownWidget>(
				                       MarioController, MatchStartingCountdownClass)
			                       : MatchStartingOverlay;
	}
	if (!MatchStartingOverlay)return;
	MatchStartingOverlay->AddToViewport();
}

void ALMMarioHUD::AddPauseOptionsOverlay()
{
	if (APlayerController* MarioController = GetOwningPlayerController())
	{
		PauseOptionsOverlay = PauseOptionsOverlay == nullptr
			                      ? CreateWidget<ULMOptionsMenu>(MarioController, PauseOptionsClass)
			                      : PauseOptionsOverlay;
	}
	if (!PauseOptionsOverlay)return;
	if (!PauseOptionsOverlay->IsInViewport())
	{
		PauseOptionsOverlay->AddToViewport();
	}
	else
	{
		PauseOptionsOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void ALMMarioHUD::HidePauseOptionsOverlay()
{
	if (!PauseOptionsOverlay || !PauseOptionsOverlay->IsInViewport())return;
	PauseOptionsOverlay->SetVisibility(ESlateVisibility::Hidden);
	if (auto* OpenedWidget = PauseOptionsOverlay->Options_Switcher->GetActiveWidget())
	{
		OpenedWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ALMMarioHUD::SetMatchInfoOverlayVisibility(const ESlateVisibility& Visibility)
{
	if (MatchOverlay)
	{
		MatchOverlay->SetVisibility(Visibility);
	}
}

void ALMMarioHUD::SetPixelRowVisibility(const ESlateVisibility& Visibility)
{
	if (MatchOverlay)
	{
		MatchOverlay->B_CurrentPixelRow->SetVisibility(Visibility);
	}
}

void ALMMarioHUD::SetAccelerometerOverlayVisibility(const ESlateVisibility& Visibility)
{
	if (DebugOverlay)
	{
		DebugOverlay->SetVisibility(Visibility);
	}
}

void ALMMarioHUD::SetPixelArtOverlayVisibility(const ESlateVisibility& Visibility)
{
	if (PixelArtOverlay)
	{
		PixelArtOverlay->SetVisibility(Visibility);
	}
}

void ALMMarioHUD::AddPostMatchOverlay()
{
	if (APlayerController* MarioController = GetOwningPlayerController())
	{
		PostMatchOverlay = PostMatchOverlay == nullptr
			                   ? CreateWidget<ULMPostMatchWidget>(MarioController, PostMatchOverlayClass)
			                   : PostMatchOverlay;
	}
	if (!PostMatchOverlay)return;
	PostMatchOverlay->AddToViewport();
}

void ALMMarioHUD::ClearAllHUD()
{
	if (MatchStartingOverlay) MatchStartingOverlay->RemoveFromParent();
	MatchStartingOverlay = nullptr;
	MatchOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (PauseOptionsOverlay) PauseOptionsOverlay->RemoveFromParent();
	PauseOptionsOverlay = nullptr;
	if (PostMatchOverlay) PostMatchOverlay->RemoveFromParent();
	PostMatchOverlay = nullptr;
}

void ALMMarioHUD::SetLayoutByTrainedHand(const EHandTrained& HandTrained)
{
	if (MatchOverlay && MatchOverlay->BTN_Pause)
	{
		if (UCanvasPanelSlot* PauseSlot = Cast<UCanvasPanelSlot>(MatchOverlay->BTN_Pause->Slot))
		{
			switch (HandTrained)
			{
			case EHandTrained::EHT_Right:
				PauseSlot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
				PauseSlot->SetAlignment(FVector2D(0.f, 0.f));
				PauseSlot->SetPosition(FVector2D(5.f, 5.f));
				break;
			default:
				PauseSlot->SetAnchors(FAnchors(1.f, 0.f, 1.f, 0.f));
				PauseSlot->SetAlignment(FVector2D(1.f, 0.f));
				PauseSlot->SetPosition(FVector2D(-5.f, 5.f));
				break;
			}
		}
	}
	if (PixelArtOverlay && PixelArtOverlay->GetPixelBorder())
	{
		if (UCanvasPanelSlot* PixelSlot = Cast<UCanvasPanelSlot>(PixelArtOverlay->GetPixelBorder()->Slot))
		{
			switch (HandTrained)
			{
			case EHandTrained::EHT_Right:
				PixelSlot->SetAnchors(FAnchors(1.f, 0.f, 1.f, 0.f));
				PixelSlot->SetAlignment(FVector2D(1.f, 0.f));
				break;
			default:
				PixelSlot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
				PixelSlot->SetAlignment(FVector2D(0.f, 0.f));
				break;
			}
			PixelSlot->SetPosition(FVector2D(0.f, 0.f));
		}
	}
}
