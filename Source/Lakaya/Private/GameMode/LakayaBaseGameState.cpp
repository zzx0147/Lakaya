#include "GameMode/LakayaBaseGameState.h"

#include "Character/LakayaBasePlayerState.h"
#include "ETC/OutlineManager.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/BattlePlayerController.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GamePlayCrossHairWidget.h"
#include "UI/GamePlayKillLogWidget.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/GameTimeWidget.h"
#include "UI/HelpWidget.h"
#include "UI/LoadingWidget.h"
#include "UI/SkillWidget.h"

ALakayaBaseGameState::ALakayaBaseGameState()
{
	PrimaryActorTick.bCanEverTick = false;
	MaximumPlayers = 6;
	MatchDuration = 180.f;
	MatchWaitDuration = 10.0f;
}

void ALakayaBaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALakayaBaseGameState, MatchEndingTime);
	DOREPLIFETIME(ALakayaBaseGameState, CharacterSelectEndingTime);
	DOREPLIFETIME(ALakayaBaseGameState, MatchWaitEndingTime);
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
				InGameTimeWidget->AddToViewport();
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

		if (CrosshairWidgetClass)
		{
			CrosshairWidget = CreateWidget<UGamePlayCrosshairWidget>(LocalController, CrosshairWidgetClass);
			if (CrosshairWidget != nullptr)
			{
				CrosshairWidget->AddToViewport();
				CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		// TODO : 아직 구현이 되지 않아 비활성화 합니다.
		// if (HelpWidgetClass)
		// {
		// 	HelpWidget = CreateWidget<UHelpWidget>(LocalController, HelpWidgetClass);
		// 	if (HelpWidget.IsValid())
		// 	{
		// 		HelpWidget->AddToViewport();
		// 		HelpWidget->SetVisibility(ESlateVisibility::Hidden);
		// 	}
		// }

		if (SkillWidgetClass)
		{
			SkillWidget = CreateWidget<USkillWidget>(LocalController, SkillWidgetClass);
			if (SkillWidget.IsValid())
			{
				SkillWidget->AddToViewport();
				SkillWidget->SetVisibility(ESlateVisibility::Hidden);

				if (const auto BattlePlayerController = Cast<ABattlePlayerController>(LocalController);
					BattlePlayerController != nullptr)
					BattlePlayerController->SetSkillWidget(SkillWidget.Get());
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

		SpawnOutlineManager();
	}
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

	if (const auto LocalPlayerState = Cast<ALakayaBasePlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
		LocalPlayerState != nullptr)
	{
		SkillWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	InternalSetCharacterSelectWidgetVisibility(false);
	if (CharacterSelectWidget) CharacterSelectWidget->EnableAutoHide(true);

	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::Hidden);

	if (InGameTimeWidget.IsValid())
		InGameTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (CrosshairWidget != nullptr)
		CrosshairWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// TODO : 아직 구현이 되지 않아 비활성화 합니다.
	// if (HelpWidget.IsValid())
	// 	HelpWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SetupTimerWidget(MatchWaitToStartTimer, MatchWaitDuration, MatchWaitEndingTime, [this]
	{
		SetupTimerWidget(EndingTimer, MatchDuration, MatchEndingTime, [this]
		{
			if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>()) AuthGameMode->EndMatch();
		}, InGameTimeWidget);
	}, InGameTimeWidget);

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(EndingTimer, [this]
		{
			if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>())
				AuthGameMode->EndMatch();
		}, MatchDuration, false);
	}
}

void ALakayaBaseGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	GetWorldTimerManager().ClearTimer(EndingTimer);

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<AGameLobbyPlayerController>();
		LocalController && LocalController->IsLocalController())
		LocalController->SetEnableExitShortcut(true);
}

void ALakayaBaseGameState::HandleMatchIsCharacterSelect()
{
	InternalSetCharacterSelectWidgetVisibility(true);

	if (LoadingWidget) LoadingWidget->SetVisibility(ESlateVisibility::Hidden);

	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	SetupTimerWidget(CharacterSelectTimer, CharacterSelectDuration, CharacterSelectEndingTime, [this]()
	{
		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>()) AuthGameMode->StartMatch();
	}, CharacterSelectTimeWidget);
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
	if (!ScoreBoard.IsValid() || MatchState != MatchState::InProgress) return;
	ScoreBoard->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

UGameLobbyCharacterSelectWidget* ALakayaBaseGameState::GetCharacterSelectWidget()
{
	// 캐릭터 위젯이 존재하지 않는 경우 생성합니다.
	if (!CharacterSelectWidget && CharacterSelectWidgetClass)
	{
		CharacterSelectWidget = CreateWidget<UGameLobbyCharacterSelectWidget>(GetWorld(), CharacterSelectWidgetClass);
		if (CharacterSelectWidget != nullptr)
		{
			CharacterSelectWidget->AddToViewport();
			CharacterSelectWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	return CharacterSelectWidget;
}

void ALakayaBaseGameState::ToggleCharacterSelectWidget()
{
	// 캐릭터 선택위젯이 숨겨져있었다면 보여주고, 보여지고 있었다면 숨깁니다.
	if (MatchState != MatchState::InProgress) return;
	InternalSetCharacterSelectWidgetVisibility(GetCharacterSelectWidget()->GetVisibility() == ESlateVisibility::Hidden);
}

void ALakayaBaseGameState::NotifyPlayerKilled_Implementation(APlayerState* VictimController,
                                                             APlayerState* InstigatorController, AActor* DamageCauser)
{
	OnPlayerKillNotified.Broadcast(VictimController, InstigatorController, DamageCauser);
	if (KillLogWidget.IsValid())
	{
		KillLogWidget->OnKillCharacterNotify(VictimController, InstigatorController, DamageCauser);
	}

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("OnPlayerKillNotified.Broadcast"));
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

void ALakayaBaseGameState::SetupTimerWidget(FTimerHandle& TimerHandle, const float& Duration, float& EndingTime,
                                            std::function<void(void)> Callback,
                                            TWeakObjectPtr<UGameTimeWidget> TimeWidget)
{
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(TimerHandle, Callback, Duration, false);

		EndingTime = GetServerWorldTimeSeconds() + Duration;
		if (TimeWidget.IsValid()) TimeWidget->SetWidgetTimer(EndingTime);
	}
}

void ALakayaBaseGameState::InternalSetScoreBoardVisibility(const bool& Visible)
{
	if (!ScoreBoard.IsValid()) return;
	ScoreBoard->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void ALakayaBaseGameState::InternalSetCharacterSelectWidgetVisibility(const bool& Visible)
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController();
		LocalController && LocalController->IsLocalController())
	{
		LocalController->SetShowMouseCursor(Visible);
		GetCharacterSelectWidget()->SetVisibility(Visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
