// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LMMatchInfoWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "GameModes/LMWhackAMoleGameMode.h"
#include "HUD/LMMarioHUD.h"
#include "HUD/PAPixelArtWidget.h"
#include "HUD/Components/PAPixelImage.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

class UPAPixelImage;

void ULMMatchInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnVisibilityChanged.AddDynamic(this, &ThisClass::HandleOnVisibilityChanged);
	WhackAMoleGameMode = WhackAMoleGameMode == nullptr
		                     ? Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(this))
		                     : WhackAMoleGameMode;
	if (!WhackAMoleGameMode)return;
	WhackAMoleGameMode->OnPixelChangedDelegate.AddDynamic(this, &ThisClass::OnPixelChanged);
}

void ULMMatchInfoWidget::HandleOnVisibilityChanged(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Visible)
	{
		WhackAMoleGameMode = WhackAMoleGameMode == nullptr
			                     ? Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(this))
			                     : WhackAMoleGameMode;
		if (!WhackAMoleGameMode)return;
		if (WhackAMoleGameMode->GetMatchState() != MatchState::PlayGame)return;
		UHorizontalBox* WidgetPixelRow = nullptr;
		if (ALMMarioController* OwnerController = Cast<ALMMarioController>(GetOwningPlayer()))
		{
			if (OwnerController->GetMarioHUD() && OwnerController->GetMarioHUD()->PixelArtOverlay)
				WidgetPixelRow =
					OwnerController->GetMarioHUD()->PixelArtOverlay->GetPixelRow(
						WhackAMoleGameMode->GetLevelPixelInfo().CurrentRow, FLinearColor(.859375f, .469344f, .056451f));
		}
		if (!WidgetPixelRow)return;
		B_PixelRow->ClearChildren();
		if (UHorizontalBox* HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass()))
		{
			for (int X = 0; X < WidgetPixelRow->GetChildrenCount(); X++)
			{
				if (UPAPixelImage* PixelImage = WidgetTree->ConstructWidget<
					UPAPixelImage>(UPAPixelImage::StaticClass()))
				{
					if (UImage* WidgetPixelImage = Cast<UImage>(WidgetPixelRow->GetChildAt(X)))
					{
						FSlateBrush PixelBrush = WidgetPixelImage->GetBrush();
						PixelImage->SetBrush(PixelBrush);
						PixelImage->SetColorAndOpacity(FColor::Black);
					}

					if (const auto HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(PixelImage)))
					{
						HorizontalBoxSlot->SetPadding(5.f);
					}
				}
			}
			PixelRow = HorizontalBox;
		}
		if (!PixelRow)return;
		B_PixelRow->AddChild(PixelRow);

		for (auto PixelCoordinate : WhackAMoleGameMode->GetLevelPixelInfo().CurrentRowPixelCoordinate)
		{
			UWidget* PixelWidget = PixelRow->GetChildAt(FMath::TruncToInt(PixelCoordinate.Y));
			if (!PixelWidget)return;
			UImage* PixelImage = Cast<UImage>(PixelWidget);
			if (!PixelImage)return;

			UMaterialInstanceDynamic* Outline = UMaterialInstanceDynamic::Create(OutlineMaterial, PixelImage);
			if (!Outline)return;
			Outline->SetVectorParameterValue(FName("BaseColor"), FLinearColor::Black);
			Outline->SetVectorParameterValue(FName("BorderColor"), FLinearColor(.5f, .5f, .5f));
			PixelImage->SetColorAndOpacity(FLinearColor::White);
			PixelImage->SetBrush(UWidgetBlueprintLibrary::MakeBrushFromMaterial(Outline, 64.f, 64.f));
		}
		for (auto PixelWidget : PixelRow->GetAllChildren())
		{
			UImage* PixelImage = Cast<UImage>(PixelWidget);
			if (!PixelImage)return;
			PixelImage->SetDesiredSizeOverride(FVector2D(64.f, 64.f));
		}
		SetCurrentPixelOutline();
	}
}

void ULMMatchInfoWidget::OnPixelChanged(bool bHit, FLinearColor TowerColor)
{
	SetLastPixelColor(bHit, TowerColor);
	SetCurrentPixelOutline();
}

void ULMMatchInfoWidget::SetCurrentPixelOutline()
{
	WhackAMoleGameMode = WhackAMoleGameMode == nullptr
		                     ? Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(this))
		                     : WhackAMoleGameMode;
	if (!WhackAMoleGameMode || !OutlineMaterial)return;
	if (WhackAMoleGameMode->GetLevelPixelInfo().CurrentRowPixelCoordinate.IsValidIndex(
		WhackAMoleGameMode->GetCurrentPixelIndex()) && PixelRow)
	{
		UWidget* CurrentPixelWidget = PixelRow->GetChildAt(
			WhackAMoleGameMode->GetLevelPixelInfo().CurrentRowPixelCoordinate[WhackAMoleGameMode->
				GetCurrentPixelIndex()].Y);
		if (!CurrentPixelWidget)return;
		CurrentPixelImage = Cast<UImage>(CurrentPixelWidget);
		if (!CurrentPixelImage)return;

		if (UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(
			CurrentPixelImage->GetBrush().GetResourceObject()))
		{
			MaterialInstance->SetVectorParameterValue(FName("BorderColor"), FLinearColor(.859375f, .469344f, .056451f));
		}
	}
}

void ULMMatchInfoWidget::SetLastPixelColor(bool bHit, FLinearColor TowerColor)
{
	if (!CurrentPixelImage)return;
	if (!bHit)
	{
		if (!MissedMaterial)return;
		UMaterialInstanceDynamic* CrossMaterial = UMaterialInstanceDynamic::Create(MissedMaterial, CurrentPixelImage);
		CrossMaterial->SetVectorParameterValue(FName("CrossColor"),
		                                       WhackAMoleGameMode->GetPlayerTurn() ==
		                                       UGameplayStatics::GetPlayerController(WhackAMoleGameMode, 0)
			                                       ? RED_COLOR
			                                       : GREEN_COLOR);
		CurrentPixelImage->SetBrush(UWidgetBlueprintLibrary::MakeBrushFromMaterial(CrossMaterial, 64.f, 64.f));
		CurrentPixelImage->SetRenderTransformAngle(45.f);
	}
	else
	{
		if (UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(
			CurrentPixelImage->GetBrush().GetResourceObject()))
		{
			FLinearColor InnerColor = TowerColor == PURPLE_COLOR
				                          ? FLinearColor(0.41662f, 0.061008f, 0.901042f)
				                          : TowerColor;
			const bool bMarioTurn = WhackAMoleGameMode->GetPlayerTurn() == UGameplayStatics::GetPlayerController(
				WhackAMoleGameMode, 0);
			if (TowerColor == BROWN_COLOR)
			{
				InnerColor = bMarioTurn
					             ? FLinearColor(0.505882f, 0.380392f, 0.031373f)
					             : FLinearColor(0.463542f, 0.119566f, 0.028786f);
			}
			MaterialInstance->SetVectorParameterValue(FName("BaseColor"), InnerColor);
			MaterialInstance->SetVectorParameterValue(FName("BorderColor"), bMarioTurn ? RED_COLOR : GREEN_COLOR);
		}
	}
}
