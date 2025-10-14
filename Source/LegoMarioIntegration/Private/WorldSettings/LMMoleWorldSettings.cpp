// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldSettings/LMMoleWorldSettings.h"

#include "EngineUtils.h"
#include "Components/AudioComponent.h"
#include "HUD/PAPixelArtWidget.h"
#include "Kismet/GameplayStatics.h"
#include "WorldPixelArts/PAWorldPixelArt.h"

void ALMMoleWorldSettings::BeginPlay()
{
	Super::BeginPlay();

	if (PixelArtTag.IsValid())
	{
		AudioComponent = Cast<UAudioComponent>(AddComponentByClass(UAudioComponent::StaticClass(), false, FTransform(), true));
		if (AudioComponent)
		{
			AudioComponent->OnAudioFinished.AddDynamic(this, &ThisClass::HandleAudioFinished);
			FString InGameMusicPath = FString::Printf(TEXT("/Game/LegoMarioIntegration/Arts/Sounds/Musics/Cues/InGameMusic_Cue"));
			USoundBase* Sound = LoadObject<USoundBase>(nullptr, *InGameMusicPath);
			AudioComponent->SetSound(Sound);
			AudioComponent->Play();
		}
	}
}

void ALMMoleWorldSettings::HandleAudioFinished()
{
	AudioComponent->Play();
}
