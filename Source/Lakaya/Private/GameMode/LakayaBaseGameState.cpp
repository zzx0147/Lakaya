// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LakayaBaseGameState.h"
#include "UI/LoadingWidget.h"

ALakayaBaseGameState::ALakayaBaseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALakayaBaseGameState::BeginPlay()
{
	Super::BeginPlay();
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		if (LoadingWidgetClass.Get() != nullptr)
		{
			LoadingWidget = CreateWidget<ULoadingWidget>(LocalController, LoadingWidgetClass);
			if (LoadingWidget != nullptr)
			{
				LoadingWidget->OnBeginPlay();
				LoadingWidget->SetMaximumPlayerNumber(6);
				LoadingWidget->SetPlayerNumber(PlayerArray.Num());
				LoadingWidget->AddToViewport();
				LoadingWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
	PlayersNumber = PlayerArray.Num();
}

void ALakayaBaseGameState::Tick(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, *GetMatchState().ToString());
}

void ALakayaBaseGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if (LoadingWidget != nullptr) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
	PlayersNumber = PlayerArray.Num();
	//if (PlayerArray.Num() >= MaximumPlayers)
}

void ALakayaBaseGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	if (LoadingWidget != nullptr) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
	PlayersNumber = PlayerArray.Num();
}

void ALakayaBaseGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	LoadingWidget->SetVisibility(ESlateVisibility::Hidden);
}
