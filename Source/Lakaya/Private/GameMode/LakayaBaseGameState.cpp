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
#include "UI/GameTimeWidget.h"
#include "UI/LoadingWidget.h"
#include "UI/OverlayMinimapWidget.h"
#include "UI/PlayerNameDisplayerWidget.h"
#include "UI/IntroWidget.h"

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
				LoadingWidget->ActivateWidget();
			}
		}

		if(InGameWidgetStackClass)
		{
			InGameWidgetStack = CreateWidget<UCommonActivatableWidget>(LocalController,InGameWidgetStackClass);
			if (InGameWidgetStack)
			{
				InGameWidgetStack->AddToViewport(3);
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

		if (IntroWidgetClass)
		{
			IntroWidget = CreateWidget<UIntroWidget>(LocalController, IntroWidgetClass);
			if (IntroWidget.IsValid())
			{
				IntroWidget->AddToViewport(1);
				IntroWidget->SetVisibility(ESlateVisibility::Hidden);
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

	if (LoadingWidget.IsValid()) LoadingWidget->DeactivateWidget();

	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SetupTimerWidget(CharacterSelectTimer, CharacterSelectDuration, CharacterSelectEndingTime, FTimerDelegate::CreateWeakLambda(this,[this]()
	{
		if (!HasAuthority()) return;

		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<ALakayaDefaultPlayGameMode>())
		{
			AuthGameMode->StartIntro();
		}
	}), CharacterSelectTimeWidget);
	

}

void ALakayaBaseGameState::HandleMatchIsIntro()
{
	if (GetCharacterSelectWidget())
	{
		CharacterSelectWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::Hidden);

	if (IntroWidget.IsValid())
		IntroWidget->SetVisibility(ESlateVisibility::Visible);
	
	FTimerDelegate IntroTimerDelegate;
	IntroTimerDelegate.BindWeakLambda(this,[&]()
	{
		if(!HasAuthority()) return;
		
		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<ALakayaDefaultPlayGameMode>())
		{
			AuthGameMode->StartMatch();
		}
	});
	
	GetWorldTimerManager().SetTimer(IntroTimer,IntroTimerDelegate,IntroDuration,false);
}

void ALakayaBaseGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	StartTimeStamp = FDateTime::UtcNow().ToUnixTimestamp();
	StartTime = GetServerWorldTimeSeconds();

	if(InGameWidgetStack)
		InGameWidgetStack->ActivateWidget();

	if (IntroWidget.IsValid())
		IntroWidget->SetVisibility(ESlateVisibility::Hidden);
	
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
	if (GetCharacterSelectWidget()) CharacterSelectWidget->RemoveFromParent();

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

bool ALakayaBaseGameState::HasMatchStarted() const
{
	if (GetMatchState() == MatchState::IsSelectCharacter)
	{
		return false;
	}
	
	return Super::HasMatchStarted();
}
