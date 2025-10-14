// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LMLevelSelectionWidget.h"

#include <comdef.h>
#include <string>

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/ButtonSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "HUD/Components/LMButton.h"
#include "HUD/Components/LMEditableTextBox.h"
#include "HUD/Components/LMSlider.h"
#include "LegoMarioIntegration/LegoMarioIntegration.h"

bool ULMLevelSelectionWidget::CreateHorizontalButtonStructure(UHorizontalBox* HBHeight, const FString& DecText, const FString& IncText, const FString& Key)
{
	FSlateChildSize SlateSizeFill;
	FSlateFontInfo Font;
	if (HBHeight)
	{
		ULMButton* DecButton = WidgetTree.Get()->ConstructWidget<ULMButton>(ULMButton::StaticClass(), FName(DecText));
		if (!DecButton)return false;
		UTextBlock* DecTextBlock = WidgetTree.Get()->ConstructWidget<UTextBlock>();
		if (!DecTextBlock)return false;
		DecTextBlock->SetText(FText::FromString("-"));
		Font = DecTextBlock->GetFont();
		Font.OutlineSettings = 2.f;
		DecTextBlock->SetFont(Font);
		UButtonSlot* DecSlot = Cast<UButtonSlot>(DecButton->AddChild(DecTextBlock));
		if (!DecSlot)return false;
		DecSlot->SetVerticalAlignment(VAlign_Center);
		DecSlot->SetHorizontalAlignment(HAlign_Center);
		DecSlot->SetPadding(FMargin(0.f));
		UHorizontalBoxSlot* HBSlot = Cast<UHorizontalBoxSlot>(HBHeight->AddChild(DecButton));
		if (!HBSlot)return false;
		HBSlot->SetVerticalAlignment(VAlign_Center);
		HBSlot->SetHorizontalAlignment(HAlign_Center);
		HBSlot->SetPadding(FMargin(0.f));
		SlateSizeFill.Value = 0.25f;
		HBSlot->SetSize(SlateSizeFill);

		UTextBlock* Text = WidgetTree.Get()->ConstructWidget<UTextBlock>();
		if (!Text)return false;
		Text->SetText(FText::FromStringTable(STRING_TABLE_FR, Key));
		Font = Text->GetFont();
		Font.Size = 16.f;
		Font.OutlineSettings = 2.f;
		Text->SetFont(Font);
		HBSlot = Cast<UHorizontalBoxSlot>(HBHeight->AddChild(Text));
		if (!HBSlot)return false;
		HBSlot->SetVerticalAlignment(VAlign_Center);
		HBSlot->SetHorizontalAlignment(HAlign_Center);
		HBSlot->SetPadding(FMargin(25.f));
		SlateSizeFill.Value = 0.5f;
		HBSlot->SetSize(SlateSizeFill);

		ULMButton* IncButton = WidgetTree.Get()->ConstructWidget<ULMButton>(ULMButton::StaticClass(), FName(IncText));
		if (!IncButton)return false;
		UTextBlock* IntText = WidgetTree.Get()->ConstructWidget<UTextBlock>();
		if (!IntText)return false;
		IntText->SetText(FText::FromString("+"));
		Font = IntText->GetFont();
		Font.OutlineSettings = 2.f;
		IntText->SetFont(Font);
		UButtonSlot* IncSlot = Cast<UButtonSlot>(IncButton->AddChild(IntText));
		if (!IncSlot)return false;
		IncSlot->SetVerticalAlignment(VAlign_Center);
		IncSlot->SetHorizontalAlignment(HAlign_Center);
		IncSlot->SetPadding(FMargin(0.f));
		HBSlot = Cast<UHorizontalBoxSlot>(HBHeight->AddChild(IncButton));
		if (!HBSlot)return false;
		HBSlot->SetVerticalAlignment(VAlign_Center);
		HBSlot->SetHorizontalAlignment(HAlign_Center);
		HBSlot->SetPadding(FMargin(0.f));
		SlateSizeFill.Value = 0.25f;
		HBSlot->SetSize(SlateSizeFill);
	}
	return true;
}

void ULMLevelSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULMLevelSelectionWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (!SB_Levels)return;
	SB_Levels->ClearChildren();

	for (int i = 0; i < FMath::CeilToInt(static_cast<float>(LevelCount) / static_cast<float>(LevelPerRow)); i++)
	{
		UHorizontalBox* HBLevels = WidgetTree.Get()->ConstructWidget<UHorizontalBox>();
		auto ScrollBoxPanel = Cast<UScrollBoxSlot>(SB_Levels->AddChild(HBLevels));
		if (!ScrollBoxPanel)return;
		FSlateChildSize SlateSizeFill = FSlateChildSize(ESlateSizeRule::Fill);
		SlateSizeFill.Value = 1.5f;
		ScrollBoxPanel->SetSize(SlateSizeFill);

		for (int j = 0; j < LevelPerRow; j++)
		{
			const int CurrentLevel = i * LevelPerRow + j;
			if (CurrentLevel >= LevelCount)break;
			UVerticalBox* VBButton = WidgetTree->ConstructWidget<UVerticalBox>();
			if (!VBButton) continue;
			UHorizontalBoxSlot* HorizontalBoxSlot = HBLevels->AddChildToHorizontalBox(VBButton);
			if (HorizontalBoxSlot)
			{
				SlateSizeFill.Value = 2.f;
				HorizontalBoxSlot->SetSize(SlateSizeFill);
				HorizontalBoxSlot->SetPadding(FMargin(20.f, 20.f, 20.f, 0.f));
			}

			UBorder* OuterBorder = WidgetTree->ConstructWidget<UBorder>();
			if (OuterBorder)
			{
				OuterBorder->SetBrushColor(FLinearColor::Black);
				OuterBorder->SetPadding(FMargin(5.f));

				UVerticalBoxSlot* VerticalBoxSlot = VBButton->AddChildToVerticalBox(OuterBorder);
				if (VerticalBoxSlot)
				{
					SlateSizeFill.Value = 0.9f;
					VerticalBoxSlot->SetSize(SlateSizeFill);
				}

				ULMButton* LevelButton = WidgetTree->ConstructWidget<ULMButton>(ULMButton::StaticClass(), FName(FString::Printf(TEXT("BTN_MoleLVL_%i"), CurrentLevel)));
				if (LevelButton)
				{
					FString LevelTexturePath = FString::Printf(TEXT("/Game/LegoMarioIntegration/Arts/Texture/LayoutTextures/LVL%03d"), CurrentLevel);
					UTexture2D* LevelTexture = LoadObject<UTexture2D>(nullptr, *LevelTexturePath);
					TArray<const FSlateBrush*> LevelTextureBrush;
					FSlateBrush Brush;
					Brush.SetResourceObject(LevelTexture);
					FButtonStyle ButtonStyle = LevelButton->GetStyle();
					ButtonStyle.SetNormal(Brush);
					ButtonStyle.SetHovered(Brush);
					ButtonStyle.SetPressed(Brush);
					LevelButton->SetStyle(ButtonStyle);
					FSlateFontInfo Font;
					UBorderSlot* OuterBorderSlot = Cast<UBorderSlot>(OuterBorder->AddChild(LevelButton));
					if (!OuterBorderSlot)return;
					OuterBorderSlot->SetPadding(FMargin(5.f));

					//
					// Set the Button Texture to the corresponding one
					//

					UBorder* InnerBorder = WidgetTree->ConstructWidget<UBorder>();
					if (InnerBorder)
					{
						InnerBorder->SetPadding(FMargin(0.f));
						InnerBorder->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.5f));
						InnerBorder->SetVisibility(OptionVisibility);

						UButtonSlot* ButtonSlot = Cast<UButtonSlot>(LevelButton->AddChild(InnerBorder));
						if (!ButtonSlot)return;
						ButtonSlot->SetHorizontalAlignment(HAlign_Fill);

						UVerticalBox* OptionVerticalBox = WidgetTree->ConstructWidget<UVerticalBox>();
						if (OptionVerticalBox)
						{
							UBorderSlot* InnerBorderSlot = Cast<UBorderSlot>(InnerBorder->AddChild(OptionVerticalBox));
							if (!InnerBorderSlot)return;
							InnerBorderSlot->SetPadding(FMargin(0.f));


							UHorizontalBox* HBNextSpawnTime = WidgetTree.Get()->ConstructWidget<UHorizontalBox>();
							if (HBNextSpawnTime)
							{
								UTextBlock* NextSpawnText = WidgetTree.Get()->ConstructWidget<UTextBlock>();
								if (!NextSpawnText)return;
								NextSpawnText->SetText(FText::FromStringTable(STRING_TABLE_FR, FString("NextSpawn")));
								Font = NextSpawnText->GetFont();
								Font.OutlineSettings.OutlineSize = 2;
								Font.Size = 16;
								NextSpawnText->SetFont(Font);
								UHorizontalBoxSlot* HBNextSpawnSlot = Cast<UHorizontalBoxSlot>(HBNextSpawnTime->AddChild(NextSpawnText));
								if (!HBNextSpawnSlot)return;
								HBNextSpawnSlot->SetVerticalAlignment(VAlign_Center);
								HBNextSpawnSlot->SetPadding(FMargin(10.f));

								ULMSlider* NextSpawnSlider = WidgetTree.Get()->ConstructWidget<ULMSlider>(ULMSlider::StaticClass(), FName(FString::Printf(TEXT("SLDR_NextSpawn_%i"), CurrentLevel)));
								if (!NextSpawnSlider)return;
								Sliders.Add(NextSpawnSlider);
								NextSpawnSlider->SetMaxValue(MaxSliderValue);
								NextSpawnSlider->SetMinValue(MinSliderValue);
								NextSpawnSlider->SetValue(DefaultValue);
								NextSpawnTime = 0.1f;
								NextSpawnSlider->OnValueChangedDelegate.AddDynamic(this, &ThisClass::HandleSliderValueChanged);
								HBNextSpawnSlot = Cast<UHorizontalBoxSlot>(HBNextSpawnTime->AddChild(NextSpawnSlider));
								if (!HBNextSpawnSlot)return;
								SlateSizeFill.Value = 0.70f;
								HBNextSpawnSlot->SetSize(SlateSizeFill);

								ULMEditableTextBox* NextSpawnEText = WidgetTree.Get()->ConstructWidget<ULMEditableTextBox>(ULMEditableTextBox::StaticClass(), FName(FString::Printf(TEXT("ETXT_NextSpawn_%i"), CurrentLevel)));
								if (!NextSpawnEText)return;
								NextSpawnSlider->LMEditableTextBox = NextSpawnEText;
								NextSpawnEText->LMSlider = NextSpawnSlider;
								NextSpawnEText->OnCustomTextCommittedDelegate.AddDynamic(this, &ThisClass::HandleETextComitted);
								NextSpawnEText->SetText(FText::FromString("0.1"));
								NextSpawnEText->SetHintText(FText::FromString("0.0"));
								NextSpawnEText->SetJustification(ETextJustify::Center);
								Font = NextSpawnEText->WidgetStyle.TextStyle.Font;
								Font.OutlineSettings.OutlineSize = 2;
								NextSpawnEText->WidgetStyle.TextStyle.SetFont(Font);
								NextSpawnEText->WidgetStyle.TextStyle.SetHighlightColor(FLinearColor::White);
								NextSpawnEText->WidgetStyle.TextStyle.SetColorAndOpacity(FLinearColor::White);
								NextSpawnEText->WidgetStyle.SetBackgroundColor(FLinearColor::Transparent);
								HBNextSpawnSlot = Cast<UHorizontalBoxSlot>(HBNextSpawnTime->AddChild(NextSpawnEText));
								if (!HBNextSpawnSlot)return;
								HBNextSpawnSlot->SetVerticalAlignment(VAlign_Center);
								HBNextSpawnSlot->SetHorizontalAlignment(HAlign_Center);
								HBNextSpawnSlot->SetPadding(FMargin(10.f, 10.f, 10.f, 10.f));
								SlateSizeFill.Value = 0.30f;
								HBNextSpawnSlot->SetSize(SlateSizeFill);
							}

							UHorizontalBox* HBHeight = WidgetTree.Get()->ConstructWidget<UHorizontalBox>();
							if (!CreateHorizontalButtonStructure(HBHeight, FString::Printf(TEXT("BTN_DecHeight_%i"), CurrentLevel), FString::Printf(TEXT("BTN_IncHeight_%i"), CurrentLevel), FString("HeightFlat")))return;
							UHorizontalBox* HBAngle = WidgetTree.Get()->ConstructWidget<UHorizontalBox>();
							if (!CreateHorizontalButtonStructure(HBAngle, FString::Printf(TEXT("BTN_DecAngle_%i"), CurrentLevel), FString::Printf(TEXT("BTN_IncAngle_%i"), CurrentLevel), FString("Angle0")))return;
							UHorizontalBox* HBMovement = WidgetTree.Get()->ConstructWidget<UHorizontalBox>();
							if (!CreateHorizontalButtonStructure(HBMovement, FString::Printf(TEXT("BTN_DecMovement_%i"), CurrentLevel), FString::Printf(TEXT("BTN_IncMovement_%i"), CurrentLevel), FString("MovementFlat")))return;

							ULMButton* StartButton = WidgetTree.Get()->ConstructWidget<ULMButton>(ULMButton::StaticClass(), FName(FString::Printf(TEXT("BTN_StartLVL_%i"), CurrentLevel)));
							if (!StartButton)return;

							UTextBlock* StartText = WidgetTree.Get()->ConstructWidget<UTextBlock>();
							if (!StartText)return;
							StartText->SetText(FText::FromStringTable(STRING_TABLE_FR, FString("Start")));
							Font = StartText->GetFont();
							Font.OutlineSettings = 2.f;
							Font.Size = 24.f;
							StartText->SetFont(Font);
							UButtonSlot* StartButtonSlot = Cast<UButtonSlot>(StartButton->AddChild(StartText));
							if (!StartButtonSlot)return;
							StartButtonSlot->SetHorizontalAlignment(HAlign_Center);
							StartButtonSlot->SetVerticalAlignment(VAlign_Center);

							UVerticalBoxSlot* VBOptionSlot = Cast<UVerticalBoxSlot>(OptionVerticalBox->AddChild(HBNextSpawnTime));
							if (!VBOptionSlot)return;
							SlateSizeFill.Value = 0.22f;
							VBOptionSlot->SetSize(SlateSizeFill);
							VBOptionSlot = Cast<UVerticalBoxSlot>(OptionVerticalBox->AddChild(HBHeight));
							if (!VBOptionSlot)return;
							SlateSizeFill.Value = 0.22f;
							VBOptionSlot->SetSize(SlateSizeFill);
							VBOptionSlot = Cast<UVerticalBoxSlot>(OptionVerticalBox->AddChild(HBAngle));
							if (!VBOptionSlot)return;
							SlateSizeFill.Value = 0.22f;
							VBOptionSlot->SetSize(SlateSizeFill);
							VBOptionSlot = Cast<UVerticalBoxSlot>(OptionVerticalBox->AddChild(HBMovement));
							if (!VBOptionSlot)return;
							SlateSizeFill.Value = 0.22f;
							VBOptionSlot->SetSize(SlateSizeFill);
							VBOptionSlot = Cast<UVerticalBoxSlot>(OptionVerticalBox->AddChild(StartButton));
							if (!VBOptionSlot)return;
							SlateSizeFill.Value = 0.16f;
							VBOptionSlot->SetSize(SlateSizeFill);
							VBOptionSlot->SetHorizontalAlignment(HAlign_Center);
							VBOptionSlot->SetVerticalAlignment(VAlign_Center);
							VBOptionSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 10.f));
						}
					}
					UTextBlock* LevelText = WidgetTree.Get()->ConstructWidget<UTextBlock>();
					if (!LevelText)return;
					LevelText->SetText(FText::FromString(FString::Printf(TEXT("Niveau %i"), (CurrentLevel) + 1)));
					Font = LevelText->GetFont();
					Font.Size = 30.f;
					LevelText->SetFont(Font);
					LevelText->SetJustification(ETextJustify::Center);

					VerticalBoxSlot = VBButton->AddChildToVerticalBox(LevelText);
					if (VerticalBoxSlot)
					{
						SlateSizeFill.Value = 0.1f;
						VerticalBoxSlot->SetSize(SlateSizeFill);
						VerticalBoxSlot->SetVerticalAlignment(VAlign_Center);
					}
				}
			}
		}
	}
}

void ULMLevelSelectionWidget::HandleSliderValueChanged(ULMEditableTextBox* EditableTextBox, float Value)
{
	EditableTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), Value)));
	NextSpawnTime = Value;
}

void ULMLevelSelectionWidget::HandleETextComitted(ULMSlider* Slider, float Value)
{
	Slider->SetValue(Value);
	NextSpawnTime = Value;
}
