// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LakayaBaseGameState.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Net/UnrealNetwork.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/GameTimeWidget.h"
#include "UI/LoadingWidget.h"

ALakayaBaseGameState::ALakayaBaseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	MaximumPlayers = 6;
	MatchDuration = 300.f;
}

void ALakayaBaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALakayaBaseGameState, MatchEndingTime);
	DOREPLIFETIME(ALakayaBaseGameState, CharacterSelectEndingTime);
}

void ALakayaBaseGameState::BeginPlay()
{
	Super::BeginPlay();
	//���� ������Ʈ�� BeginPlay������ LocalController�� PlayerState�� �׻� �������� ������� ���� ���⼭�� ���� ��Ʈ�ѷ��� PlayerState�� ���������� �ϸ� �ȵ�
	
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

		if (InGameTimerWidgetClass)
		{
			InGameTimeWidget = CreateWidget<UGameTimeWidget>(LocalController, InGameTimerWidgetClass);
			if (InGameTimeWidget.IsValid())
			{
				InGameTimeWidget->AddToViewport();
				InGameTimeWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (CharacterSelectTimerWidgetClass)
		{
			CharacterSelectTimeWidget = CreateWidget<UGameTimeWidget>(LocalController, CharacterSelectTimerWidgetClass);
			if (CharacterSelectTimeWidget.IsValid())
			{
				CharacterSelectTimeWidget->AddToViewport();
				CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::Hidden);
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
	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::Hidden);
	if (InGameTimeWidget.IsValid())
		InGameTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SetupTimerWidget(EndingTimer, MatchDuration, MatchEndingTime, [this] {
		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>()) AuthGameMode->EndMatch(); }
	, InGameTimeWidget);

}

void ALakayaBaseGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	GetWorldTimerManager().ClearTimer(EndingTimer);
}

void ALakayaBaseGameState::HandleMatchIsCharacterSelect()
{
	LoadingWidget->SetVisibility(ESlateVisibility::Hidden);
	CharacterSelectWidget->SetVisibility(ESlateVisibility::Visible);
	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SetupTimerWidget(CharacterSelectTimer, CharacterSelectDuration, CharacterSelectEndingTime, [this]() {
		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>()) AuthGameMode->StartMatch(); }
	, CharacterSelectTimeWidget);


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
				CharacterSelectWidget->OnChangeSelectedCharacter.AddLambda([this](const FName& Name) {
					if (MatchState == MatchState::InProgress) SetCharacterSelectWidgetVisibility(ESlateVisibility::Hidden);
					});
			}
		}
	}
}

void ALakayaBaseGameState::SetCharacterSelectWidgetVisibility(const ESlateVisibility& IsVisible)
{
	if (CharacterSelectWidget != nullptr && MatchState == MatchState::InProgress)
	{
		CharacterSelectWidget->SetVisibility(IsVisible);
		if (const auto PlayerController = GetWorld()->GetFirstPlayerController<APlayerController>())
		{
			PlayerController->SetShowMouseCursor(IsVisible == ESlateVisibility::Visible);
		}
	}


}

ESlateVisibility ALakayaBaseGameState::GetCharacterSelectWidgetVisibility() const
{
	if (CharacterSelectWidget != nullptr)
		return CharacterSelectWidget->GetVisibility();

	return ESlateVisibility::Hidden;
}

void ALakayaBaseGameState::OnRep_MatchEndingTime()
{
	if (InGameTimeWidget.IsValid()) InGameTimeWidget->SetWidgetTimer(MatchEndingTime);
}

void ALakayaBaseGameState::OnRep_CharacterSelectEndingTime()
{
	if (CharacterSelectTimeWidget.IsValid()) CharacterSelectTimeWidget->SetWidgetTimer(CharacterSelectEndingTime);
}

void ALakayaBaseGameState::SetupTimerWidget(FTimerHandle& TimerHandle, const float& Duration, float& EndingTime, std::function<void(void)> Callback, TWeakObjectPtr<UGameTimeWidget> TimeWidget)
{
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(TimerHandle, Callback, Duration, false);

		EndingTime = GetServerWorldTimeSeconds() + Duration;
		if (TimeWidget.IsValid()) TimeWidget->SetWidgetTimer(EndingTime);
	}
}
