// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Components/LMButton.h"

#include "Binding/States/WidgetStateRegistration.h"
#include "Components/ButtonSlot.h"
#include "Datas/LMLayoutTowers.h"

TSharedRef<SWidget> ULMButton::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	MyButton = SNew(SButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleCustomClicked))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::HandleCustomHovered)
		.OnUnhovered_UObject(this, &ThisClass::HandleCustomUnHovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}

	return MyButton.ToSharedRef();
}

void ULMButton::HandleCustomHovered()
{
	OnButtonHoveredDelegate.Broadcast(this);
	BroadcastBinaryPostStateChange(UWidgetHoveredStateRegistration::Bit, true);
}

void ULMButton::HandleCustomUnHovered()
{
	OnButtonUnHoveredDelegate.Broadcast(this);
	BroadcastBinaryPostStateChange(UWidgetHoveredStateRegistration::Bit, false);
}

void ULMButton::HandleCustomClicked()
{
	OnButtonClickedDelegate.Broadcast(this);
	BroadcastBinaryPostStateChange(UWidgetPressedStateRegistration::Bit, true);
}

void ULMButton::GatherLevelPixelTags()
{
	if (DT_Layout)
	{
		TArray<FLayoutActivatedTowers*> TableRows;
		FString Name = GetName();
		int Index = Name.Len() - 1;
		int Value = FCString::Atoi(&Name[Index]);
		DT_Layout->GetAllRows<FLayoutActivatedTowers>(FString(),TableRows);
		LevelTags = ULMLayoutTowers::GetLevelRelatedPixelTags(TableRows, Value);
	}
}
