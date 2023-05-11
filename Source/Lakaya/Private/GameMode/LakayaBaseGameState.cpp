// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LakayaBaseGameState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/LoadingWidget.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "Character/LakayaBasePlayerState.h"

ALakayaBaseGameState::ALakayaBaseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	MaximumPlayers = 6;
}


void ALakayaBaseGameState::BeginPlay()
{
	Super::BeginPlay();
	//게임 스테이트의 BeginPlay에서는 LocalController의 PlayerState가 항상 존재함이 보장되지 않음 여기서는 로컬 컨트롤러의 PlayerState를 가져오려고 하면 안됨
	
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		if (LoadingWidgetClass.Get() != nullptr)
		{
			LoadingWidget = CreateWidget<ULoadingWidget>(LocalController, LoadingWidgetClass);
			if (LoadingWidget != nullptr)
			{
				LoadingWidget->SetMaximumPlayerNumber(MaximumPlayers);
				LoadingWidget->SetPlayerNumber(PlayerArray.Num());
				LoadingWidget->AddToViewport();
				LoadingWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}


		if (ScoreBoardClass)
		{
			ScoreBoard = CreateWidget<UGameScoreBoardWidget>(LocalController, ScoreBoardClass);
			if (ScoreBoard.IsValid())
			{
				ScoreBoard->AddToViewport();
				ScoreBoard->SetVisibility(ESlateVisibility::Hidden);
				for (auto& Player : PlayerArray) ScoreBoard->RegisterPlayer(Player);
			}
		}
	}
}

void ALakayaBaseGameState::Tick(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, *GetMatchState().ToString());
}

void ALakayaBaseGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if (LoadingWidget != nullptr) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	if (ScoreBoard.IsValid()) ScoreBoard->RegisterPlayer(PlayerState);
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
}

void ALakayaBaseGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	if (LoadingWidget != nullptr) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
}

void ALakayaBaseGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	CharacterSelectWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ALakayaBaseGameState::HandleMatchIsCharacterSelect()
{
	LoadingWidget->SetVisibility(ESlateVisibility::Hidden);
	CharacterSelectWidget->SetVisibility(ESlateVisibility::Visible);

	//if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	//	CreateCharacterSelectWidget(LocalController);//위젯이 아직 없으면 생성함
}

void ALakayaBaseGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();
	if (MatchState == MatchState::IsSelectCharacter)
	{
		HandleMatchIsCharacterSelect();
	}
}

void ALakayaBaseGameState::SetScoreBoardVisibility(const bool& Visible)
{
	if (ScoreBoard.IsValid())
		ScoreBoard->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void ALakayaBaseGameState::CreateCharacterSelectWidget(APlayerController* LocalController)
{
	//로컬 컨트롤러와 PlayerState는 존재함이 보장됨, 서버의 경우 BeginPlay애서 클라이언트의 경우 OnRep_PlayerState에서 호출해줌
	if (CharacterSelectWidget != nullptr) return;

	if (CharacterSelectWidgetClass.Get() != nullptr)
	{
		CharacterSelectWidget = CreateWidget<UGameLobbyCharacterSelectWidget>(LocalController, CharacterSelectWidgetClass);
		if (CharacterSelectWidget != nullptr)
		{
			CharacterSelectWidget->AddToViewport();
			CharacterSelectWidget->SetVisibility(ESlateVisibility::Hidden);

			if (const auto BasePlayerState = LocalController->GetPlayerState<ALakayaBasePlayerState>())
			{
				CharacterSelectWidget->OnChangeSelectedCharacter.AddUObject(BasePlayerState, &ALakayaBasePlayerState::RequestCharacterChange);
			}
		}
	}
}
