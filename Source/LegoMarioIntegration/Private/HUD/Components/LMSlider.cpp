// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Components/LMSlider.h"

#include "Widgets/Input/SSlider.h"

TSharedRef<SWidget> ULMSlider::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	MySlider = SNew(SSlider)
		.Style(&WidgetStyle)
		.IsFocusable(IsFocusable)
		.OnMouseCaptureBegin(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnMouseCaptureBegin))
		.OnMouseCaptureEnd(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnMouseCaptureEnd))
		.OnControllerCaptureBegin(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnControllerCaptureBegin))
		.OnControllerCaptureEnd(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnControllerCaptureEnd))
		.OnValueChanged(BIND_UOBJECT_DELEGATE(FOnFloatValueChanged, HandleCustomValueChanged));
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	return MySlider.ToSharedRef();
}

void ULMSlider::HandleCustomValueChanged(float InValue)
{
	SetValue(InValue);
	OnValueChangedDelegate.Broadcast(LMEditableTextBox, GetValue());
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::Value);
}
