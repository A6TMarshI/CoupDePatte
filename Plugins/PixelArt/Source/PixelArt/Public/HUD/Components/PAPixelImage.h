// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "PAPixelImage.generated.h"

class UHorizontalBox;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPAImageClickedDelegate, UImage*, Image);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPAImageClearedDelegate, UImage*, Image);

/**
 * 
 */
UCLASS()
class PIXELART_API UPAPixelImage : public UImage
{
	GENERATED_BODY()

public:
	FReply HandleColorMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FSlateColor GetPixelColorAndOpacity() const;
	const FSlateBrush* GetPixelBrush() const;
	virtual void SynchronizeProperties() override;

	UPROPERTY()
	FPAImageClickedDelegate OnImageClickedDelegate;
	UPROPERTY()
	FPAImageClearedDelegate OnImageClearedDelegate;
};
