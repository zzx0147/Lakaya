#include "GameMode/LakayaBaseGameState.h"

#include "Character/LakayaBasePlayerState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Net/UnrealNetwork.h"
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

		if (HelpWidgetClass)
		{
			HelpWidget = CreateWidget<UHelpWidget>(LocalController, HelpWidgetClass);
			if (HelpWidget.IsValid())
			{
				HelpWidget->AddToViewport();
				HelpWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (SkillWidgetClass)
		{
			SkillWidget = CreateWidget<USkillWidget>(LocalController, SkillWidgetClass);
			if (SkillWidget.IsValid())
			{
				SkillWidget->AddToViewport();
				SkillWidget->SetVisibility(ESlateVisibility::Hidden);
			}				
		}

		if(KillLogWidgetClass)
		{
			KillLogWidget = CreateWidget<UGamePlayKillLogWidget>(LocalController, KillLogWidgetClass);
			if (KillLogWidget.IsValid())
			{
				KillLogWidget->AddToViewport();
				KillLogWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
		
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

	const auto LocalPlayerState = Cast<ALakayaBasePlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	if (LocalPlayerState != nullptr)
	{
		const FString CharacterName = LocalPlayerState->GetCharacterName().ToString();

		if (CharacterName == "Rena")
		{
			// TODO : 레나 캐릭터 전용 UI
			if (SkillWidget != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("레나 캐릭터입니다."));
				SkillWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SkillWidget->RenaQSkill->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SkillWidget->RenaESkill->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SkillWidget->RenaRButtonSkill->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SkillWidget->RenaRSkill->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}

		if (CharacterName == "Wazi")
		{
			// TODO : 와지 캐릭터 전용 UI
			if (SkillWidget != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("와지 캐릭터입니다."));
				SkillWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SkillWidget->WaziQSkill->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SkillWidget->WaziESkill->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SkillWidget->WaziRButtonSkill->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SkillWidget->WaziRSkill->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
	
	if(CharacterSelectWidget != nullptr)
		CharacterSelectWidget->SetVisibility(ESlateVisibility::Hidden);
	
	if (CharacterSelectTimeWidget.IsValid())
		CharacterSelectTimeWidget->SetVisibility(ESlateVisibility::Hidden);
	
	if (InGameTimeWidget.IsValid())
		InGameTimeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	if (CrosshairWidget != nullptr)
		CrosshairWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (HelpWidget.IsValid())
		HelpWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	if (const auto PlayerController = GetWorld()->GetFirstPlayerController())
		PlayerController->SetShowMouseCursor(false);

	SetupTimerWidget(EndingTimer, MatchDuration, MatchEndingTime, [this] {
		if (const auto AuthGameMode = GetWorld()->GetAuthGameMode<AGameMode>()) AuthGameMode->EndMatch(); }
	, InGameTimeWidget);

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
}

void ALakayaBaseGameState::HandleMatchIsCharacterSelect()
{
	if (const auto PlayerController = GetWorld()->GetFirstPlayerController())
		PlayerController->SetShowMouseCursor(true);

	if (LoadingWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget is null."));
	}
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

	//TODO: 중첩 분기문 들여쓰기를 줄입시다. 바로 위의 if문에 합칠 수 있을 것 같아요.
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

void ALakayaBaseGameState::OnLocalPlayerControllerCreated(APlayerController* LocalPlayerController)
{
	CreateCharacterSelectWidget(LocalPlayerController);
}

//TODO: 다른 클래스에서 픽창 위젯의 비저빌리티를 체크하는 건 이상합니다.
ESlateVisibility ALakayaBaseGameState::GetCharacterSelectWidgetVisibility() const
{
	if (CharacterSelectWidget != nullptr)
		return CharacterSelectWidget->GetVisibility();

	return ESlateVisibility::Hidden;
}

void ALakayaBaseGameState::NotifyPlayerKilled_Implementation(APlayerState* VictimController, APlayerState* InstigatorController, AActor* DamageCauser)
{
	//TODO : OnPlayerKillNofity.BroadCast();
	OnPlayerKillNotified.Broadcast(VictimController, InstigatorController, DamageCauser);
	if (KillLogWidget.IsValid())
	{
		KillLogWidget->OnKillCharacterNotify(VictimController,InstigatorController,DamageCauser);
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

void ALakayaBaseGameState::SetupTimerWidget(FTimerHandle& TimerHandle, const float& Duration, float& EndingTime, std::function<void(void)> Callback, TWeakObjectPtr<UGameTimeWidget> TimeWidget)
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