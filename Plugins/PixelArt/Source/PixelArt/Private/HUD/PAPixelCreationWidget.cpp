// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PAPixelCreationWidget.h"

#if WITH_EDITOR
#include "GameplayTagsEditorModule.h"
#endif
#include "GameplayTagsManager.h"
#include "PixelArt.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/EditableText.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Datas/PALoadMatLab.h"
#include "Helpers/PAReadWriteFile.h"
#include "HUD/Components/PAPixelImage.h"
#include "Internationalization/Text.h"

TArray<FPAPixelArtDataInfos>::SizeType FPAPixelArtDataInfos::FindByTag(const TArray<FPAPixelArtDataInfos>& Array, const FGameplayTag& Tag)
{
	const TArray<FPAPixelArtDataInfos>::ElementType* RESTRICT Start = Array.GetData();
	for (const TArray<FPAPixelArtDataInfos>::ElementType * RESTRICT Data = Start, *RESTRICT DataEnd = Data + Array.Num(); Data != DataEnd; ++Data)
	{
		if (Data->PixelArtTag == Tag)
		{
			return static_cast<TArray<FPAPixelArtDataInfos>::SizeType>(Data - Start);
		}
	}
	return INDEX_NONE;
}

void UPAPixelCreationWidget::GeneratePixelArtScale()
{
	if (!WidgetTree || !VB_Row)return;
	VB_Row->ClearChildren();

	for (int X = 0; X < Row; X++)
	{
		FName Name = FName("Border_" + FString::FromInt(X));
		if (UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name))
		{
			Border->SetBrushColor(FLinearColor::Transparent);
			Name = FName("HorizontalBox_" + FString::FromInt(X));
			if (UHorizontalBox* HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), Name))
			{
				for (int Y = 0; Y < Column; Y++)
				{
					Name = FName("PixelImage_" + FString::FromInt(X) + "_" + FString::FromInt(Y));
					if (UPAPixelImage* PixelImage = WidgetTree->ConstructWidget<UPAPixelImage>(UPAPixelImage::StaticClass(), Name))
					{
						FSlateBrush PixelBrush = PixelImage->GetBrush();
						PixelBrush.SetImageSize(FVector2D(16.f, 16.f));
						PixelImage->SetBrush(PixelBrush);
						PixelImage->OnImageClickedDelegate.AddDynamic(this, &ThisClass::ColorOnMouseButtonDown);
						PixelImage->OnImageClearedDelegate.AddDynamic(this, &ThisClass::ClearOnMouseButtonDown);
						if (const auto HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(PixelImage)))
						{
							HorizontalBoxSlot->SetPadding(FMargin(1.f, 0.f));
						}
					}
				}
				if (const auto BorderSlot = Cast<UBorderSlot>(Border->AddChild(HorizontalBox))) BorderSlot->SetPadding(2.f);
				VB_Row->AddChild(Border);
			}
		}
	}
}

bool UPAPixelCreationWidget::AddNewGameplayTag()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	// Only support adding tags via ini file
	if (Manager.ShouldImportTagsFromINI() == false)
	{
		return false;;
	}

	FText TagNameAsText = ETXT_GameplayTag->GetText();
	FString TagName = TagNameAsText.ToString();

	if (TagName.IsEmpty())
	{
		return false;;
	}

#if WITH_EDITOR
	if (!IGameplayTagsEditorModule::Get().AddNewGameplayTagToINI(TagName, FString(), FName("DefaultGameplayTags.ini")))return false;
	PixelTag = Manager.RequestGameplayTag(FName(TagName));
	return true;
#endif

	return false;
}

bool UPAPixelCreationWidget::RemoveGameplayTag()
{
#if WITH_EDITOR
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	const auto TagToRemove = Manager.FindTagNode(PixelTag);
	if (!IGameplayTagsEditorModule::Get().DeleteTagFromINI(TagToRemove))return false;
	return true;
#endif

	return false;
}

FGameplayTag UPAPixelCreationWidget::RequestPixelArtTag(const FName& TagName)
{
	const UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	return TagManager.RequestGameplayTag(TagName);
}

void UPAPixelCreationWidget::SynchronizePixelProperties()
{
	SynchronizeProperties();
	const auto PixelArtArray = UPAReadWriteFile::ReadJsonToMultipleStruct(FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH);

	const int Index = FPAPixelArtDataInfos::FindByTag(PixelArtArray, PixelTag);
	if (Index != INDEX_NONE) GeneratePixelArtFromJson(PixelArtArray[Index], false);
	else GeneratePixelArtScale();
}

void UPAPixelCreationWidget::ColorOnMouseButtonDown(UImage* Image)
{
	if (Image)
	{
		Image->SetColorAndOpacity(Color);
	}
}

void UPAPixelCreationWidget::ClearOnMouseButtonDown(UImage* Image)
{
	if (Image)
	{
		Image->SetColorAndOpacity(FLinearColor::White);
	}
}

void UPAPixelCreationWidget::UpdateColor()
{
	if (ETXT_R && ETXT_G && ETXT_B && IMG_ColorSample)
	{
		FString Red = ETXT_R->GetText().ToString();
		FString Green = ETXT_G->GetText().ToString();
		FString Blue = ETXT_B->GetText().ToString();

		Red = Red.Replace(TEXT(","),TEXT("."));
		Green = Green.Replace(TEXT(","),TEXT("."));
		Blue = Blue.Replace(TEXT(","),TEXT("."));

		const float R = FCString::Atof(*Red);
		const float G = FCString::Atof(*Green);
		const float B = FCString::Atof(*Blue);

		Color = FLinearColor(R, G, B);
		IMG_ColorSample->SetColorAndOpacity(Color);
	}
}

bool UPAPixelCreationWidget::SavePixelArt()
{
#if WITH_EDITOR
	const FGameplayTag TagNone = FGameplayTag::RequestGameplayTag(TAG_NONE);
	if (PixelTag.MatchesTag(TagNone) || !VB_Row)return false;;
	TArray<FPAPixelArtDataInfos> PixelArtArray = UPAReadWriteFile::ReadJsonToMultipleStruct(FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH);
	FPAPixelArtDataInfos PixelArtDataInfos = FPAPixelArtDataInfos();
	for (int Y = 0; Y < VB_Row->GetChildrenCount(); Y++)
	{
		if (const auto Border = Cast<UBorder>(VB_Row->GetChildAt(Y)))
		{
			if (const auto HorizontalBox = Cast<UHorizontalBox>(Border->GetChildAt(0)))
			{
				for (int X = 0; X < HorizontalBox->GetChildrenCount(); X++)
				{
					if (const auto PixelImage = Cast<UPAPixelImage>(HorizontalBox->GetChildAt(X)))
					{
						PixelArtDataInfos.PixelColorMap.Add(FVector2D(Y, X), {PixelImage->GetColorAndOpacity(), FLinearColor::Black, PixelImage->GetColorAndOpacity() == FLinearColor::Black ? EPixelState::EPS_Hidden : EPixelState::EPS_Locked});
					}
				}
			}
		}
	}
	PixelArtDataInfos.PixelArtTag = PixelTag;
	PixelArtDataInfos.Columns = Column;
	PixelArtDataInfos.Rows = Row;
	const int Index = FPAPixelArtDataInfos::FindByTag(PixelArtArray, PixelTag);
	if (Index != INDEX_NONE)PixelArtArray[Index] = PixelArtDataInfos;
	else return false;

	if (UPAReadWriteFile::CreatePixelArtSave(FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH, PixelArtArray))return true;
#endif
	return false;
}

bool UPAPixelCreationWidget::SaveAsPixelArt()
{
#if WITH_EDITOR
	const FGameplayTag TagNone = FGameplayTag::RequestGameplayTag(TAG_NONE);
	if (PixelTag.MatchesTag(TagNone) || !VB_Row)return false;;
	TArray<FPAPixelArtDataInfos> PixelArtArray = UPAReadWriteFile::ReadJsonToMultipleStruct(FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH);

	const int Index = FPAPixelArtDataInfos::FindByTag(PixelArtArray, PixelTag);
	if (Index != INDEX_NONE)return false;

	FPAPixelArtDataInfos PixelArtDataInfos = FPAPixelArtDataInfos();
	for (int Y = 0; Y < VB_Row->GetChildrenCount(); Y++)
	{
		if (const auto Border = Cast<UBorder>(VB_Row->GetChildAt(Y)))
		{
			if (const auto HorizontalBox = Cast<UHorizontalBox>(Border->GetChildAt(0)))
			{
				for (int X = 0; X < HorizontalBox->GetChildrenCount(); X++)
				{
					if (const auto PixelImage = Cast<UPAPixelImage>(HorizontalBox->GetChildAt(X)))
					{
						PixelArtDataInfos.PixelColorMap.Add(FVector2D(Y, X), {PixelImage->GetColorAndOpacity(), FLinearColor::Black, PixelImage->GetColorAndOpacity() == FLinearColor::Black ? EPixelState::EPS_Hidden : EPixelState::EPS_Locked});
					}
				}
			}
		}
	}
	PixelArtDataInfos.PixelArtTag = PixelTag;
	PixelArtDataInfos.Columns = Column;
	PixelArtDataInfos.Rows = Row;
	PixelArtArray.Add(PixelArtDataInfos);

	if (UPAReadWriteFile::CreatePixelArtSave(FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH, PixelArtArray))return true;
#endif
	return false;
}

bool UPAPixelCreationWidget::LoadPixelArt()
{
	if (!DataTable)return false;
	TArray<FMatLabInfo*> MatLabInfos;
	DataTable->GetAllRows<FMatLabInfo>(FString(), MatLabInfos);
	if (!WidgetTree || !VB_Row)return false;;
	VB_Row->ClearChildren();
	int LastRow = 0;
	Column = MatLabInfos[MatLabInfos.Num() - 1]->Y + 1;
	FName Name = FName("Border_" + FString::FromInt(LastRow));
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name);
	Border->SetBrushColor(FLinearColor::Transparent);
	Name = FName("HorizontalBox_" + FString::FromInt(LastRow));
	UHorizontalBox* HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), Name);
	for (auto MatLabInfo : MatLabInfos)
	{
		if (MatLabInfo->X != LastRow)
		{
			if (const auto BorderSlot = Cast<UBorderSlot>(Border->AddChild(HorizontalBox))) BorderSlot->SetPadding(2.f);
			VB_Row->AddChild(Border);
			LastRow++;
			Name = FName("Border_" + FString::FromInt(MatLabInfo->X));
			Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name);
			Border->SetBrushColor(FLinearColor::Transparent);
			Name = FName("HorizontalBox_" + FString::FromInt(MatLabInfo->X));
			HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), Name);
		}
		Name = FName("PixelImage_" + FString::FromInt(MatLabInfo->X) + "_" + FString::FromInt((MatLabInfo->Y)));
		if (UPAPixelImage* PixelImage = WidgetTree->ConstructWidget<UPAPixelImage>(UPAPixelImage::StaticClass(), Name))
		{
			FSlateBrush PixelBrush = PixelImage->GetBrush();
			PixelBrush.SetImageSize(FVector2D(16.f, 16.f));
			PixelImage->SetBrush(PixelBrush);
			PixelImage->SetColorAndOpacity(FLinearColor{MatLabInfo->R, MatLabInfo->G, MatLabInfo->B});
			PixelImage->OnImageClickedDelegate.AddDynamic(this, &ThisClass::ColorOnMouseButtonDown);
			PixelImage->OnImageClearedDelegate.AddDynamic(this, &ThisClass::ClearOnMouseButtonDown);
			if (const auto HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(PixelImage)))
			{
				HorizontalBoxSlot->SetPadding(FMargin(1.f, 0.f));
			}
		}
	}
	if (const auto BorderSlot = Cast<UBorderSlot>(Border->AddChild(HorizontalBox))) BorderSlot->SetPadding(2.f);
	VB_Row->AddChild(Border);
	Row = ++LastRow;
	return true;
}

void UPAPixelCreationWidget::GeneratePixelArtFromJson(const FPAPixelArtDataInfos& PixelArt, const bool& bOverrideScale)
{
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
			if (const auto HB = Cast<UHorizontalBox>(Border->GetChildAt(0)))
			{
				if (const auto Pixel = Cast<UPAPixelImage>(HB->GetChildAt(ColorPair.Key.Y))) Pixel->SetColorAndOpacity(ColorPair.Value.Color);
			}
		}
	}
}

bool UPAPixelCreationWidget::TryGetPixelFromJson(FPAPixelArtDataInfos& PixelArt)
{
	const TArray<FPAPixelArtDataInfos> PixelArtArray = UPAReadWriteFile::ReadJsonToMultipleStruct(FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH);

	const int Index = FPAPixelArtDataInfos::FindByTag(PixelArtArray, PixelTag);
	if (Index == INDEX_NONE) return false;
	PixelArt = PixelArtArray[Index];
	return true;
}

bool UPAPixelCreationWidget::TryGetPixelFromJson(FPAPixelArtDataInfos& PixelArt, const FGameplayTag& PixelArtTag)
{
	const TArray<FPAPixelArtDataInfos> PixelArtArray = UPAReadWriteFile::ReadJsonToMultipleStruct(FPaths::ProjectContentDir() + PIXEL_ART_FILE_PATH);

	const int Index = FPAPixelArtDataInfos::FindByTag(PixelArtArray, PixelArtTag);
	if (Index == INDEX_NONE) return false;
	PixelArt = PixelArtArray[Index];
	return true;
}

void UPAPixelCreationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FPAPixelArtDataInfos PixelArt;
	if (TryGetPixelFromJson(PixelArt))GeneratePixelArtFromJson(PixelArt, false);

	const UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	const FGameplayTag ParentTag = FGameplayTag::RequestGameplayTag(PIXEL_ART_PARENT_TAG);
	AvailablePixelArtTags = TagManager.RequestGameplayTagChildren(ParentTag);
	OnNativeConstructFinishedDelegate.Broadcast();
}
