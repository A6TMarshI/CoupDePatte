// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Components/LMEditableTextBox.h"

#include "HUD/Components/LMSlider.h"

TSharedRef<SWidget> ULMEditableTextBox::RebuildWidget()
{
	MyEditableTextBlock = SNew(SEditableTextBox)
		.Style(&WidgetStyle)
		.IsReadOnly(GetIsReadOnly())
		.IsPassword(GetIsPassword())
		.MinDesiredWidth(GetMinimumDesiredWidth())
		.IsCaretMovedWhenGainFocus(GetIsCaretMovedWhenGainFocus())
		.SelectAllTextWhenFocused(true)
		.RevertTextOnEscape(true)
		.ClearKeyboardFocusOnCommit(GetClearKeyboardFocusOnCommit())
		.SelectAllTextOnCommit(GetSelectAllTextOnCommit())
		.AllowContextMenu(AllowContextMenu)
		.OnTextChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnTextChanged))
		.OnTextCommitted(BIND_UOBJECT_DELEGATE(FOnTextCommitted, CustomHandleTextCommitted))
		.VirtualKeyboardType(EVirtualKeyboardType::AsKeyboardType(KeyboardType.GetValue()))
		.VirtualKeyboardOptions(VirtualKeyboardOptions)
		.VirtualKeyboardTrigger(VirtualKeyboardTrigger)
		.VirtualKeyboardDismissAction(VirtualKeyboardDismissAction)
		.Justification(GetJustification())
		.OverflowPolicy(GetTextOverflowPolicy());

	return MyEditableTextBlock.ToSharedRef();
}

void ULMEditableTextBox::CustomHandleTextCommitted(const FText& InText, ETextCommit::Type Commit)
{
	if (!InText.IdenticalTo(InText, ETextIdenticalModeFlags::DeepCompare | ETextIdenticalModeFlags::LexicalCompareInvariants))
	{
		SetText(InText);
		BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::Text);
	}
	if (!InText.IsNumeric())
	{
		SetText(FText::FromString(""));
		return;
	}
	OnCustomTextCommittedDelegate.Broadcast(LMSlider, FMath::Clamp(FCString::Atof(*InText.ToString()), LMSlider->GetMinValue(), LMSlider->GetMaxValue()));
}
