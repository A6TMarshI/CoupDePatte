// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldPixelArts/PAWorldPixelArt.h"


APAWorldPixelArt::APAWorldPixelArt()
{
	PrimaryActorTick.bCanEverTick = false;

	PixelArtWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PixelArtWidgetComponent"));
	PixelArtWidgetComponent->SetupAttachment(RootComponent);
}

void APAWorldPixelArt::BeginPlay()
{
	Super::BeginPlay();
	
}
