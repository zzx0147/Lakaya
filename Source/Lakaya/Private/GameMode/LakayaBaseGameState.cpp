#include "GameMode/LakayaBaseGameState.h"

#include <filesystem>

#include "TabMinimapWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "EOS/EOSGameInstance.h"
#include "ETC/OutlineManager.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/BattlePlayerController.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GamePlayKillLogWidget.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/GameTimeWidget.h"
#include "UI/OverlayMinimapWidget.h"
#include "UI/LoadingWidget.h"
#include "UI/PlayerNameDisplayerWidget.h"

ALakayaBaseGameState::ALakayaBaseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	MaximumPlayers = 6;
	MatchDuration = 180.f;
	MatchWaitDuration = 30.0f;
	bWantsSendRecordResult = false;
}

void ALakayaBaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALakayaBaseGameState, MatchEndingTime);
	DOREPLIFETIME(ALakayaBaseGameState, MatchWaitEndingTime);
	DOREPLIFETIME(ALakayaBaseGameState, CharacterSelectEndingTime);
}

void ALakayaBaseGameState::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto LocalController = GetWorld()->GetFirstPlayerController();
		LocalController && LocalController->IsLocalController())
	{
		if (LoadingWidgetClass.Get() != nullptr)
		{
			LoadingWidget = CreateWidget<ULoadingWidget>(LocalController, LoadingWidgetClass);
			if (LoadingWidget != nullptr)
			{
				LoadingWidget->SetMaximumPlayerNumber(MaximumPlayers);
				LoadingWidget->SetPlayerNumber(PlayerArray.Num());
				LoadingWidget->AddToViewport(10);
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
				InGameTimeWidget->AddToViewport(10);
				InGameTimeWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (CharacterSelectTimerWidgetClass)
		{
			CharacterSelectTimeWidget = CreateWidget<UGameTimeWidget>(LocalController, CharacterSelectTimerWidgetClass);
			if (CharacterSelectTimeWidget.IsValid())
			{
				CharacterSelectTimeWidget->AddToViewport(10);
				CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		
		if (KillLogWidgetClass)
		{
			KillLogWidget = CreateWidget<UGamePlayKillLogWidget>(LocalController, KillLogWidgetClass);
			if (KillLogWidget.IsValid())
			{
				KillLogWidget->AddToViewport();
				KillLogWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
		
		if (PlayerNameDisplayerWidgetClass)
		{
			PlayerNameDisplayerWidget = CreateWidget<UPlayerNameDisplayerWidget>(LocalController, PlayerNameDisplayerWidgetClass);
			if (PlayerNameDisplayerWidget.IsValid())
			{
				PlayerNameDisplayerWidget->AddToViewport();
				PlayerNameDisplayerWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
	}
	
	SpawnOutlineManager();
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

void ALakayaBaseGameState::HandleMatchIsCharacterSelect()
{
	if (GetCharacterSelectWidget()) CharacterSelectWidget->SetVisibility(ESlateVisibility::Visible);

	if (LoadingWidget.IsValid()) LoadingWidget->SetVisibility(ESlateVisibility::Hidden);

	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SetupTimerWidget(CharacterSelectTimer, CharacterSelectDuration, CharacterSelectEndingTime, [this]()
	{
		if (!HasAuthority()) return;
		
		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>())
		{
			AuthGameMode->StartMatch();
		}
	}, CharacterSelectTimeWidget);
}

void ALakayaBaseGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	StartTimeStamp = FDateTime::UtcNow().ToUnixTimestamp();
	StartTime = GetServerWorldTimeSeconds();
	
	if (GetCharacterSelectWidget())
	{
		CharacterSelectWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::Hidden);
	
	if (InGameTimeWidget.IsValid())
	{
		InGameTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	SetupTimerWidget(MatchWaitToStartTimer, MatchWaitDuration, MatchWaitEndingTime, [this]
	{
		SetupTimerWidget(EndingTimer, MatchDuration, MatchEndingTime, [this]
		{
			if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>())
			{
				AuthGameMode->EndMatch();
			}
		}, InGameTimeWidget);
	}, InGameTimeWidget);
	
}

void ALakayaBaseGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	GetWorldTimerManager().ClearTimer(EndingTimer);
	if (ScoreBoard.IsValid()) ScoreBoard->RemoveFromParent();
	if (GetCharacterSelectWidget()) CharacterSelectWidget->RemoveFromParent();

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<AGameLobbyPlayerController>();
		LocalController && LocalController->IsLocalController())
		LocalController->SetEnableExitShortcut(true);

	const auto GameInstance = GetGameInstance();
	const auto EOSGameInstance = Cast<UEOSGameInstance>(GameInstance);
	if (EOSGameInstance == nullptr) return;
	if (HasAuthority())
	{
		// EOSGameInstance->DestroySession();
	}
	else
	{
		EOSGameInstance->EndSession();
	}
}

void ALakayaBaseGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bWantsSendRecordResult)
	{
		bWantsSendRecordResult = !TrySendMatchResultData();
	}
}

void ALakayaBaseGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();
	if (MatchState == MatchState::IsSelectCharacter)
	{
		HandleMatchIsCharacterSelect();
	}

	if (MatchState != MatchState::InProgress)
	{
		if (HUDMinimapWidget) HUDMinimapWidget->SetAllyUpdateMinimap(false);
		if (TabMinimapWidget) TabMinimapWidget->SetAllyUpdateMinimap(false);
	}
}

void ALakayaBaseGameState::SetScoreBoardVisibility(const bool& Visible)
{
}

void ALakayaBaseGameState::SetTabMinimapVisibility(const bool& Visible)
{
}

void ALakayaBaseGameState::RequestClairvoyanceActivate(const AActor* InInstigator)
{
	if (!CanInstigatorClairvoyance(InInstigator))
	{
		return;
	}

	OnClairvoyanceActivateRequested(InInstigator);

	if (ShouldActivateClairvoyance())
	{
		bIsClairvoyanceActivated = true;
		OnClairvoyanceActivated();
	}
}

void ALakayaBaseGameState::RequestClairvoyanceDeactivate(const AActor* InInstigator)
{
	OnClairvoyanceDeactivateRequested(InInstigator);

	if (!ShouldActivateClairvoyance())
	{
		bIsClairvoyanceActivated = false;
		OnClairvoyanceDeactivated();
	}
}

UGameLobbyCharacterSelectWidget* ALakayaBaseGameState::GetCharacterSelectWidget()
{
	// 캐릭터 위젯이 존재하지 않는 경우 생성합니다.
	if (!CharacterSelectWidget.IsValid() && CharacterSelectWidgetClass)
	{
		CharacterSelectWidget = CreateWidget<UGameLobbyCharacterSelectWidget>(GetWorld(), CharacterSelectWidgetClass);
		if (CharacterSelectWidget != nullptr)
		{
			CharacterSelectWidget->AddToViewport();
			CharacterSelectWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	return CharacterSelectWidget.Get();
}

void ALakayaBaseGameState::NotifyPlayerKilled_Implementation(APlayerState* VictimPlayer,
                                                             APlayerState* InstigatorPlayer, AActor* DamageCauser)
{
	OnPlayerKillNotified.Broadcast(VictimPlayer, InstigatorPlayer, DamageCauser);
	if (KillLogWidget.IsValid())
	{
		KillLogWidget->OnKillCharacterNotify(VictimPlayer, InstigatorPlayer, DamageCauser);
	}

	if(const auto InstigatorBasePlayerState = Cast<ALakayaBasePlayerState>(InstigatorPlayer))
	{
		InstigatorBasePlayerState->OnKillOtherPlayer();
	}
	
}

void ALakayaBaseGameState::OnRep_MatchEndingTime()
{
	if (InGameTimeWidget.IsValid()) InGameTimeWidget->SetWidgetTimer(MatchEndingTime);
}

void ALakayaBaseGameState::OnRep_CharacterSelectEndingTime()
{
	if (CharacterSelectTimeWidget.IsValid()) CharacterSelectTimeWidget->SetWidgetTimer(CharacterSelectEndingTime);
}

void ALakayaBaseGameState::OnRep_MatchWaitEndingTime()
{
	if (InGameTimeWidget.IsValid()) InGameTimeWidget->SetWidgetTimer(MatchWaitEndingTime);
}

bool ALakayaBaseGameState::SpawnOutlineManager()
{
	if (OutlineManager.IsValid())
		return true;

	if (OutlineManagerClass)
	{
		OutlineManager = GetWorld()->SpawnActor<AOutlineManager>(OutlineManagerClass);
		if (OutlineManager.IsValid())
			return true;
	}
	return false;
}

void ALakayaBaseGameState::ReserveSendRecord()
{
	bWantsSendRecordResult = true;
}

bool ALakayaBaseGameState::TrySendMatchResultData()
{
	return true;
}

bool ALakayaBaseGameState::CanInstigatorClairvoyance(const AActor* InInstigator) const
{
	return ensure(InInstigator);
}

void ALakayaBaseGameState::OnClairvoyanceActivated()
{
	OutlineManager->SetClairvoyance(true);
}

void ALakayaBaseGameState::OnClairvoyanceDeactivated()
{
	OutlineManager->SetClairvoyance(false);
}

void ALakayaBaseGameState::SetupTimerWidget(FTimerHandle& TimerHandle, const float& Duration, float& EndingTime,
                                            std::function<void(void)> Callback,
                                            TWeakObjectPtr<UGameTimeWidget> TimeWidget)
{
	GetWorldTimerManager().SetTimer(TimerHandle, Callback, Duration, false);
	if (HasAuthority())
	{
		EndingTime = GetServerWorldTimeSeconds() + Duration;
		if (TimeWidget.IsValid()) TimeWidget->SetWidgetTimer(EndingTime);
	}
}

bool ALakayaBaseGameState::HasMatchStarted() const
{
	if (GetMatchState() == MatchState::IsSelectCharacter)
	{
		return false;
	}
	
	return Super::HasMatchStarted();
}
