// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PAPixelCreationWidget.h"
#include "Components/Border.h"
#include "PAPixelArtWidget.generated.h"

/**
 * 
 */
UCLASS()
class PIXELART_API UPAPixelArtWidget : public UPAPixelCreationWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	virtual void GeneratePixelArtFromJson(const FPAPixelArtDataInfos& PixelArt, const bool& bOverrideScale) override;

	UFUNCTION(BlueprintCallable)
	void SetPixelArtTag(const FPAPixelArtDataInfos& PixelArtDataInfos);

	UFUNCTION(BlueprintCallable)
	UHorizontalBox* GetPixelRow(const int RequestedRow, const FLinearColor& DesiredColor);

	UPROPERTY(meta=(BindWidget))
	UBorder* B_Pixel;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UBorder* GetPixelBorder() const { return B_Pixel; }
};
