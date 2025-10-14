// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PAPixelArtWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "HUD/Components/PAPixelImage.h"

void UPAPixelArtWidget::NativeConstruct()
{
	Super::Super::NativeConstruct();
}

void UPAPixelArtWidget::GeneratePixelArtFromJson(const FPAPixelArtDataInfos& PixelArt, const bool& bOverrideScale)
{
	Super::GeneratePixelArtFromJson(PixelArt, bOverrideScale);

	if (!bOverrideScale)
	{
		Column = PixelArt.Columns;
		Row = PixelArt.Rows;
	}
	GeneratePixelArtScale();
	for (const auto ColorPair : PixelArt.PixelColorMap)
	{
		if (const auto Border = Cast<UBorder>(VB_Row->GetChildAt(ColorPair.Key.X)))
		{
			if (const auto HB = Cast<UHorizontalBox>(Border->GetChildAt(Border->GetChildrenCount() - 1)))
			{
				if (const auto Pixel = Cast<UPAPixelImage>(HB->GetChildAt(ColorPair.Key.Y))) Pixel->SetColorAndOpacity(ColorPair.Value.PixelState == EPixelState::EPS_Unlocked ? ColorPair.Value.HitColor : FLinearColor::Black);
			}
		}
	}
}

void UPAPixelArtWidget::SetPixelArtTag(const FPAPixelArtDataInfos& PixelArtDataInfos)
{
	PixelTag = PixelArtDataInfos.PixelArtTag;
	GeneratePixelArtFromJson(PixelArtDataInfos, false);
}

UHorizontalBox* UPAPixelArtWidget::GetPixelRow(const int RequestedRow, const FLinearColor& DesiredColor)
{
	if (!VB_Row)return nullptr;
	int Index = VB_Row->GetChildrenCount() - RequestedRow - 1;
	UBorder* RequestedBorder = Cast<UBorder>(VB_Row->GetChildAt(Index));
	if (!RequestedBorder)return nullptr;
	UHorizontalBox* RequestedPixelRow = Cast<UHorizontalBox>(RequestedBorder->GetChildAt(RequestedBorder->GetChildrenCount() - 1));
	RequestedBorder->SetBrushColor(DesiredColor);
	return RequestedPixelRow;
}
