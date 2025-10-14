// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "PAWorldPixelArt.generated.h"


UCLASS()
class PIXELART_API APAWorldPixelArt : public AActor
{
	GENERATED_BODY()

public:
	APAWorldPixelArt();

	UPROPERTY(EditAnywhere, Category="PixelArt", BlueprintReadOnly)
	UWidgetComponent* PixelArtWidgetComponent;

protected:
	virtual void BeginPlay() override;
};
