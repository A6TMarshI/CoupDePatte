// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Components/PAPixelImage.h"

FReply UPAPixelImage::HandleColorMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == FKey(EKeys::LeftMouseButton))
	{
		OnImageClickedDelegate.Broadcast(this);
	}
	else if (MouseEvent.GetEffectingButton() == FKey(EKeys::RightMouseButton))
	{
		OnImageClearedDelegate.Broadcast(this);
	}


	return FReply::Handled();
}

FSlateColor UPAPixelImage::GetPixelColorAndOpacity() const
{
	return GetColorAndOpacity();
}

const FSlateBrush* UPAPixelImage::GetPixelBrush() const
{
	return &GetBrush();
}

void UPAPixelImage::SynchronizeProperties()
{
	Super::Super::SynchronizeProperties();


	TAttribute<FSlateColor> ColorAndOpacityBinding = TAttribute<FSlateColor>::Create(
		TAttribute<FSlateColor>::FGetter::CreateUObject(this, &ThisClass::GetPixelColorAndOpacity));

	TAttribute<const FSlateBrush*> ImageBinding = TAttribute<const FSlateBrush*>::Create(
		TAttribute<const FSlateBrush*>::FGetter::CreateUObject(this, &ThisClass::GetPixelBrush));

	if (MyImage.IsValid())
	{
		MyImage->SetImage(ImageBinding);
		MyImage->SetColorAndOpacity(ColorAndOpacityBinding);
		MyImage->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleColorMouseButtonDown));
	}
}
