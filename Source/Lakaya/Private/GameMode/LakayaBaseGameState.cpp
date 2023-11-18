#include "GameMode/LakayaBaseGameState.h"

#include "TabMinimapWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "EOS/EOSGameInstance.h"
#include "ETC/OutlineManager.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/LakayaPlayerController.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GamePlayKillLogWidget.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/GameStateSequentialWidget.h"
#include "UI/GameTimeWidget.h"
#include "UI/IntroWidget.h"
#include "UI/LoadingWidget.h"
#include "UI/OverlayMinimapWidget.h"
#include "UI/PlayerNameDisplayerWidget.h"

ALakayaBaseGameState::ALakayaBaseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	MaximumPlayers = 6;
	MatchDuration = 180.f;
	MatchWaitDuration = 30.0f;
	bWantsSendRecordResult = false;
	IntroDuration = 5.0f;
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

	if (const auto Sequential = GetOrCreateSequentialWidget())
	{
		LoadingWidget = Sequential->GetLoadingWidget();
		if (LoadingWidget.IsValid())
		{
			LoadingWidget->SetMaximumPlayerNumber(MaximumPlayers);
			LoadingWidget->SetPlayerNumber(PlayerArray.Num());
		}
		SequentialWidget->SwitchToLoading();

		// 캐릭터 선택창은 이전에 이미 세팅되었을 수 있습니다.
		if (!CharacterSelectWidget.IsValid())
		{
			CharacterSelectWidget = Sequential->GetCharacterSelectWidget();
		}

		if (CharacterSelectWidget.IsValid())
		{
			CharacterSelectTimeWidget = CharacterSelectWidget->GetTimerWidget();
		}

		IntroWidget = Sequential->GetGameIntroWidget();
		InGameWidgetStack = Sequential->GetInGameOverlayWidget();
	}

	if (const auto LocalController = GetWorld()->GetFirstPlayerController();
		LocalController && LocalController->IsLocalController())
	{
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
			PlayerNameDisplayerWidget = CreateWidget<UPlayerNameDisplayerWidget>(
				LocalController, PlayerNameDisplayerWidgetClass);
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
	if (LoadingWidget.IsValid()) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	if (ScoreBoard.IsValid()) ScoreBoard->RegisterPlayer(PlayerState);
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
}

void ALakayaBaseGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	if (LoadingWidget.IsValid()) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
}

void ALakayaBaseGameState::HandleMatchIsCharacterSelect()
{
	if (IsValid(SequentialWidget))
	{
		SequentialWidget->SwitchToCharacterSelect();
	}

	SetupTimerWidget(CharacterSelectTimer, CharacterSelectDuration, CharacterSelectEndingTime, FTimerDelegate::CreateWeakLambda(this,[this]
	{
		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<ALakayaDefaultPlayGameMode>())
		{
			AuthGameMode->StartIntro();
		}
	}), CharacterSelectTimeWidget);
}

void ALakayaBaseGameState::HandleMatchIsIntro()
{
	if (IsValid(SequentialWidget))
	{
		SequentialWidget->SwitchToGameIntro();
	}

	FTimerDelegate IntroTimerDelegate;
	IntroTimerDelegate.BindWeakLambda(this, [this]
	{
		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<ALakayaDefaultPlayGameMode>())
		{
			AuthGameMode->StartMatch();
		}
	});

	GetWorldTimerManager().SetTimer(IntroTimer, IntroTimerDelegate, IntroDuration, false);
}

void ALakayaBaseGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	StartTimeStamp = FDateTime::UtcNow().ToUnixTimestamp();
	StartTime = GetServerWorldTimeSeconds();

	if (SequentialWidget)
	{
		SequentialWidget->SwitchToInGameOverlay();
	}
	
	//TODO: 타임 위젯은 인게임 위젯에 어태치하도록 해야 합니다.
	if (InGameTimeWidget.IsValid())
	{
		InGameTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	SetupTimerWidget(MatchWaitToStartTimer, MatchWaitDuration, MatchWaitEndingTime,FTimerDelegate::CreateWeakLambda(this, [this]
	{
		SetupTimerWidget(EndingTimer, MatchDuration, MatchEndingTime,FTimerDelegate::CreateWeakLambda(this, [this]
		{
			if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>())
			{
				AuthGameMode->EndMatch();
			}
		}), InGameTimeWidget);
	}), InGameTimeWidget);
	
}

void ALakayaBaseGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	GetWorldTimerManager().ClearTimer(EndingTimer);
	if (ScoreBoard.IsValid()) ScoreBoard->RemoveFromParent();
	if (IsValid(SequentialWidget))
	{
		//TODO: 매치 종료 위젯으로 스위칭할 수도 있게 해야 합니다.
		SequentialWidget->RemoveFromParent();
	}

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<ALakayaPlayerController>();
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
	else if(MatchState == MatchState::IsIntro)
	{
		HandleMatchIsIntro();		
	}

	if (MatchState != MatchState::InProgress)
	{
		if (HUDMinimapWidget) HUDMinimapWidget->SetUpdateMinimap(false);
		if (TabMinimapWidget) TabMinimapWidget->SetUpdateMinimap(false);
	}
}

void ALakayaBaseGameState::SetScoreBoardVisibility(const bool& Visible)
{
}

void ALakayaBaseGameState::SetTabMinimapVisibility(const bool& Visible)
{
}

UGameLobbyCharacterSelectWidget* ALakayaBaseGameState::GetOrCreateCharacterSelectWidget()
{
	// 위젯이 없으면 시퀀셜 위젯을 통해서 가져옵니다.
	if (!CharacterSelectWidget.IsValid())
	{
		if (const auto Sequential = GetOrCreateSequentialWidget())
		{
			CharacterSelectWidget = Sequential->GetCharacterSelectWidget();
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

void ALakayaBaseGameState::SetupTimerWidget(FTimerHandle& TimerHandle, const float& Duration, float& EndingTime,
                                            const FTimerDelegate& Callback,
                                            TWeakObjectPtr<UGameTimeWidget> TimeWidget)
{
	GetWorldTimerManager().SetTimer(TimerHandle, Callback, Duration, false);
	if (HasAuthority())
	{
		EndingTime = GetServerWorldTimeSeconds() + Duration;
		if (TimeWidget.IsValid()) TimeWidget->SetWidgetTimer(EndingTime);
	}
}

UGameStateSequentialWidget* ALakayaBaseGameState::GetOrCreateSequentialWidget()
{
	if (IsValid(SequentialWidget))
	{
		return SequentialWidget;
	}

	// Create Widget
	if (const auto PC = GetGameInstance()->GetFirstLocalPlayerController(); PC && ensure(SequentialWidgetClass))
	{
		SequentialWidget = CreateWidget<UGameStateSequentialWidget>(PC, SequentialWidgetClass);
		if (IsValid(SequentialWidget))
		{
			SequentialWidget->AddToViewport(10);
			SequentialWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			return SequentialWidget;
		}
	}
	return nullptr;
}

bool ALakayaBaseGameState::HasMatchStarted() const
{
	if (GetMatchState() == MatchState::IsSelectCharacter)
	{
		return false;
	}
	
	return Super::HasMatchStarted();
}
