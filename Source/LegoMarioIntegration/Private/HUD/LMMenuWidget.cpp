// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LMMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void ULMMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<UButton*> Buttons = FindAllButtons();

	for (auto Button : Buttons)
	{
		Button->OnHovered.AddDynamic(this, &ThisClass::PlayHoveredSound);
		Button->OnClicked.AddDynamic(this, &ThisClass::PlayClickedSound);
	}
}

void ULMMenuWidget::PlayHoveredSound()
{
	if (HoveredSound.Get())
	{
		UGameplayStatics::PlaySound2D(this, HoveredSound.Get());
	}
}

void ULMMenuWidget::PlayClickedSound()
{
	if (ClickedSound.Get())
	{
		UGameplayStatics::PlaySound2D(this, ClickedSound.Get());
	}
}

void ULMMenuWidget::AddButton(UPanelWidget* CurrentWidget, TArray<UButton*>& OutButtons)
{
	if (!CurrentWidget)return;

	if (UButton* Button = Cast<UButton>(CurrentWidget))
	{
		OutButtons.Add(Button);
	}
}

TArray<UButton*> ULMMenuWidget::FindAllButtons()
{
	TArray<UButton*> Buttons;

	TArray<UWidget*> Widgets;
	this->WidgetTree.Get()->GetAllWidgets(Widgets);
	for (const auto Widget : Widgets)
	{
		if (const auto Panel = Cast<UPanelWidget>(Widget))
		{
			AddButton(Panel, Buttons);
		}
	}

	return Buttons;
}
