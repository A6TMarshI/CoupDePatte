// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstances/LMGameInstance.h"

#include "Blueprint/WidgetTree.h"
#include "GameModes/LMMarioGameMode.h"
#include "HUD/LMMarioHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Socket/LMSocket.h"
#include "Socket/LMTCPSocketListener.h"


ULMGameInstance::ULMGameInstance()
{
	SocketComponent = CreateDefaultSubobject<ULMSocket>("MarioSocketComponent");
}

void ULMGameInstance::BindMarioSocket(ALMMarioController* InController)
{
	if (!InController)return;
	SocketComponent->BindToLego(InController);
}

void ULMGameInstance::BindLuigiSocket(ALMMarioController* InController)
{
	if (!InController)return;
	SocketComponent->BindToLego(InController);
}

bool ULMGameInstance::IsReadyForBinding()
{
	for (auto PendingConnection : SocketComponent->PendingConnections)
	{
		if (PendingConnection.LegoListenerThread->GetLegoName().IsEmpty())return false;
	}
	return SocketComponent->PendingConnections.Num() == 2;
}

void ULMGameInstance::PauseThread(bool bPause)
{
	for (auto PendingConnection : SocketComponent->PendingConnections)
	{
		PendingConnection.LegoListenerThread->Pause(bPause);
	}
}


void ULMGameInstance::OnStart()
{
	Super::OnStart();

	OnConnectionEstablishedDelegate.AddDynamic(this, &ThisClass::HandleLegoConnection);

	CreateSocketConnection();
}

void ULMGameInstance::Shutdown()
{
	Super::Shutdown();

	SocketComponent->DestroySocketComponents(true);
}

void ULMGameInstance::CreateSocketConnection()
{
	SocketComponent->InitializeSocket(this);
}

void ULMGameInstance::HandleLegoConnection(FString Name, bool bConnected)
{
	const auto World = GetWorld();
	if (!World)return;
	const auto PC = World->GetFirstPlayerController();
	if (!PC)return;
	const auto HUD = Cast<ALMMarioHUD>(PC->GetHUD());
	if (!HUD)return;

	if (!bConnected)SocketComponent->DestroySocketListenerByName(Name);
	if (IsReadyForBinding())
	{
		BindMarioSocket(Cast<ALMMarioController>(UGameplayStatics::GetPlayerController(this, 0)));
		BindLuigiSocket(Cast<ALMMarioController>(UGameplayStatics::GetPlayerController(this, 1)));
		if (auto GameMode = Cast<ALMWhackAMoleGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			if (GameMode->GetMatchState() == MatchState::PlayGame)return;
			GameMode->OnAllControllerConnectedDelegate.Broadcast();
		}
	}
}
