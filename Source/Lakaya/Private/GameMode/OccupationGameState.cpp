#include "GameMode/OccupationGameState.h"

#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "ETC/OutlineManager.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "GameMode/OccupationGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Occupation/ShieldWallObject.h"
#include "UI/FinalResultWidget.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GameResultWidget.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/GameTimeWidget.h"
#include "UI/MatchStartWaitWidget.h"
#include "UI/OccupationCharacterSelectWidget.h"
#include "UI/HUDOccupationMinimapWidget.h"
#include "UI/StartMessageWidget.h"
#include "UI/TeamScoreWidget.h"
#include "UI/WeaponOutLineWidget.h"
#include "UI/OccupyExpressWidget.h"
#include "UI/PlayerNameDisplayerWidget.h"

void AOccupationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationGameState, AntiTeamScore);
	DOREPLIFETIME(AOccupationGameState, ProTeamScore);
	DOREPLIFETIME(AOccupationGameState, CurrentOccupationWinner);
	DOREPLIFETIME(AOccupationGameState, AntiTeamCaptureAreaCount);
	DOREPLIFETIME(AOccupationGameState, ProTeamCaptureAreaCount);
	DOREPLIFETIME(AOccupationGameState, TeamToUpdate);
}

AOccupationGameState::AOccupationGameState(): MatchResult()
{
	MaxScore = 100.f;
	AntiTeamScore = 0.0f;
	ProTeamScore = 0.0f;
	AntiTeamCaptureAreaCount = 0;
	ProTeamCaptureAreaCount = 0;
	ScoreUpdateDelay = 1.0f;
	AdditiveScore = 3.0f;
	MatchDuration = 180.f;
	MatchStartWaitWidgetLifeTime = 3.0f;
	MatchStartWidgetLifeTime = 5.0f;
	ClientTeam = ETeam::None;
	CurrentOccupationWinner = ETeam::None;

	PlayersByTeamMap.Emplace(ETeam::Anti);
	PlayersByTeamMap.Emplace(ETeam::Pro);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ResultContextFinder(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IC_ResultWidgetControl.IC_ResultWidgetControl'"));

	static ConstructorHelpers::FObjectFinder<UInputAction> ResultSwitchActionFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ShowScore.IA_ShowScore'"));

	ResultShortcutContext = ResultContextFinder.Object;
	ResultSwitchingAction = ResultSwitchActionFinder.Object;

	HUDMinimapWidgetClass = UHUDOccupationMinimapWidget::StaticClass();
	TabMinimapWidgetClass = UOccupationTabMinimapWidget::StaticClass();
}

void AOccupationGameState::BeginPlay()
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		LocalController && LocalController->IsLocalController())
	{
		if (TeamScoreWidgetClass)
		{
			TeamScoreWidget = CreateWidget<UTeamScoreWidget>(LocalController, TeamScoreWidgetClass);
			if (TeamScoreWidget)
			{
				TeamScoreWidget->AddToViewport();
				TeamScoreWidget->SetVisibility(ESlateVisibility::Hidden);
				TeamScoreWidget->SetMaxScore(MaxScore);
				TeamScoreWidget->SetMaxScoreVisibility(false);
				OnTeamScoreSignature.AddUObject(TeamScoreWidget,&UTeamScoreWidget::SetTeamScore);
			}
			else UE_LOG(LogTemp, Warning, TEXT("TeamScoreWidget is null."));
		}

		if (WeaponOutLineWidgetClass)
		{
			WeaponOutLineWidget = CreateWidget<UWeaponOutLineWidget>(LocalController, WeaponOutLineWidgetClass);
			if (IsValid(WeaponOutLineWidget))
			{
				WeaponOutLineWidget->AddToViewport();
				WeaponOutLineWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (MatchStartWaitWidgetClass)
		{
			MatchStartWaitWidget = CreateWidget<UMatchStartWaitWidget>(LocalController, MatchStartWaitWidgetClass);
			if (MatchStartWaitWidget.IsValid())
			{
				MatchStartWaitWidget->AddToViewport(1);
				MatchStartWaitWidget->SetVisibility(ESlateVisibility::Hidden);
			}
			else UE_LOG(LogTemp, Warning, TEXT("MatchStartWaitWidget is null."));
		}

		if (StartMessageWidgetClass)
		{
			StartMessageWidget = CreateWidget<UStartMessageWidget>(LocalController, StartMessageWidgetClass);
			if (StartMessageWidget.IsValid())
			{
				StartMessageWidget->AddToViewport(1);
				StartMessageWidget->SetVisibility(ESlateVisibility::Hidden);
			}
			else UE_LOG(LogTemp, Warning, TEXT("StartMessageWidget is null."));
		}

		if (GameResultWidgetClass)
		{
			GameResultWidget = CreateWidget<UGameResultWidget>(LocalController, GameResultWidgetClass);
			if (GameResultWidget.IsValid())
			{
				GameResultWidget->AddToViewport();
				GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (FinalResultWidgetClass)
		{
			FinalResultWidget = CreateWidget<UFinalResultWidget>(
				LocalController, FinalResultWidgetClass);
			if (FinalResultWidget)
			{
				FinalResultWidget->AddToViewport(30);
				FinalResultWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (OccupyExpressWidgetClass)
		{
			OccupyExpressWidget = CreateWidget<UOccupyExpressWidget>(
				LocalController, OccupyExpressWidgetClass);
			if (OccupyExpressWidget.IsValid())
			{
				OccupyExpressWidget->AddToViewport();
				OccupyExpressWidget->SetVisibility(ESlateVisibility::Hidden);
				OccupyBarMaps.Emplace(1, OccupyExpressWidget->GetAntiBar());
				OccupyBarMaps.Emplace(2, OccupyExpressWidget->GetCenterBar());
				OccupyBarMaps.Emplace(3, OccupyExpressWidget->GetProBar());
			}
		}

		if (HUDMinimapWidgetClass)
		{
			HUDMinimapWidget = CreateWidget<UHUDOccupationMinimapWidget>(
				LocalController, HUDMinimapWidgetClass);
			if (HUDMinimapWidget)
			{
				HUDMinimapWidget->AddToViewport();
				HUDMinimapWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (TabMinimapWidgetClass)
		{
			TabMinimapWidget = CreateWidget<UOccupationTabMinimapWidget>(
				LocalController, TabMinimapWidgetClass);
			if (TabMinimapWidget)
			{
				TabMinimapWidget->AddToViewport();
				TabMinimapWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle_GameTimeCheck, this,
	                                &AOccupationGameState::EndTimeCheck, 1.0f, true);

	Super::BeginPlay();
}

void AOccupationGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (IsValid(TeamScoreWidget))
		TeamScoreWidget->SetVisibility(ESlateVisibility::Visible);

	if (IsValid(WeaponOutLineWidget))
		WeaponOutLineWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (OccupyExpressWidget.IsValid())
		OccupyExpressWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (HUDMinimapWidget)
	{
		HUDMinimapWidget->SetUpdateMinimap(true);
		HUDMinimapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
			LocalController && LocalController->IsLocalController())
		{
			if (const auto LakayaPlayerState = LocalController->GetPlayerState<ALakayaBasePlayerState>())
			{
				HUDMinimapWidget->SetTeam(LakayaPlayerState->GetTeam());

				// 와지가 스킬을 쓰게 되면 상대방의 위치도 미니맵 상에 업데이트 해줘야 하기 때문에
				// 일단은 본인의 팀과 상대방의 팀의 정보를 미니맵 위젯에 넣어주도록 합니다.
				UpdatePlayerByMinimap(ETeam::Anti, LakayaPlayerState);
				UpdatePlayerByMinimap(ETeam::Pro, LakayaPlayerState);
			}
		}
	}

	if (TabMinimapWidget)
	{
		TabMinimapWidget->SetUpdateMinimap(true);

		if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
			LocalController && LocalController->IsLocalController())
		{
			if (const auto LakayaPlayerState = LocalController->GetPlayerState<ALakayaBasePlayerState>())
			{
				TabMinimapWidget->SetTeam(LakayaPlayerState->GetTeam());

				// 와지가 스킬을 쓰게 되면 상대방의 위치도 미니맵 상에 업데이트 해줘야 하기 때문에
				// 본인의 팀과 상대팀의 정보를 미니맵 위젯에 넣어주도록 합니다.
				UpdatePlayerByMinimap(ETeam::Anti, LakayaPlayerState);
				UpdatePlayerByMinimap(ETeam::Pro, LakayaPlayerState);
			}
		}
	}
	
	FTimerDelegate TimerDelegate_MatchStartWaitWidget;
	TimerDelegate_MatchStartWaitWidget.BindLambda([this]
	{
		if (this == nullptr) return;
		if (MatchStartWaitWidget.IsValid())
			MatchStartWaitWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	});
	GetWorldTimerManager().SetTimer(TimerHandle_MatchStartWaitWidget, TimerDelegate_MatchStartWaitWidget,
	                                MatchWaitDuration - 10, false);

	// MatchStartWaitWidget위젯을 띄우고, N초(MatchStartWaitWidgetLifeTime) 뒤에 비활성화 해줍니다.
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]
	{
		if (this == nullptr) return;
		if (MatchStartWaitWidget.IsValid()) MatchStartWaitWidget->SetVisibility(ESlateVisibility::Hidden);
	});
	GetWorldTimerManager().SetTimer(TimerHandle_WaitTimerHandle, TimerDelegate,
	                                (MatchWaitDuration - 10 + MatchStartWaitWidgetLifeTime), false);

	// 게임이 본격적으로 시작이 되면 StartMessage위젯을 띄워줍니다.
	TimerDelegate.BindWeakLambda(this,[this]
	{
		if (StartMessageWidget.IsValid()) StartMessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		DestroyShieldWallObject();
		if(TeamScoreWidget) TeamScoreWidget->SetMaxScoreVisibility(true);
		if(InGameTimeWidget.IsValid()) InGameTimeWidget->SetVisibility(ESlateVisibility::Hidden);
	});
	GetWorldTimerManager().SetTimer(TimerHandle_StartMessageVisible, TimerDelegate, MatchWaitDuration, false);

	// StartMessage위젯을 5초 뒤에 비활성화 해줍니다.
	TimerDelegate.BindWeakLambda(this,[this]
	{
		if (StartMessageWidget.IsValid()) StartMessageWidget->SetVisibility(ESlateVisibility::Hidden);
	});
	
	GetWorldTimerManager().SetTimer(TimerHandle_StartMessageHidden, TimerDelegate,
	                                MatchWaitDuration + MatchStartWidgetLifeTime, false);
}

void AOccupationGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	if (GameResultWidget.IsValid())
		GameResultWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// Anti팀의 배열과, Pro팀의 배열을 내림차순으로 정렬합니다.
	static auto Predicate = [](const ALakayaBasePlayerState& A, const ALakayaBasePlayerState& B)
	{
		return A.GetTotalScore() > B.GetTotalScore();
	};

	for (auto& Element : PlayersByTeamMap)
		Element.Value.Sort(Predicate);

	MatchResult.WinTeam = GetOccupationWinner();
	MatchResult.StartTime = StartTimeStamp;
	MatchResult.Duration = GetServerWorldTimeSeconds() - StartTime;
	AddPlayerStateToRecordResult(ETeam::Anti, PlayersByTeamMap[ETeam::Anti]);
	AddPlayerStateToRecordResult(ETeam::Pro, PlayersByTeamMap[ETeam::Pro]);

	if (HasAuthority())
	{
		if (const auto EOSGameInstance = Cast<UEOSGameInstance>(GetGameInstance()))
		{
			ReserveSendRecord();
		}
	}

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		LocalController && LocalController->IsLocalController())
	{
		GameResultWidget->ShowResult(ClientTeam == GetOccupationWinner(), AntiTeamScore, ProTeamScore);

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&]
		{
			GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
			FinalResultWidget->SetTeam(ClientTeam);
			FinalResultWidget->SetMatchResultData(GetOccupationWinner(), ProTeamScore, AntiTeamScore, PlayersByTeamMap);
			FinalResultWidget->SetVisibility(ESlateVisibility::Visible);
		});
		GetWorldTimerManager().SetTimer(TimerHandle_GameResultHandle, TimerDelegate, 5.0f, false);
	}
}

void AOccupationGameState::EndTimeCheck()
{
	if (AntiTeamScore >= MaxScore || ProTeamScore >= MaxScore)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_GameTimeCheck);

		auto GameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
			return;
		}

		GameMode->EndMatch();
	}
}

void AOccupationGameState::SetOccupationWinner()
{
	CurrentOccupationWinner = AntiTeamScore > ProTeamScore ? ETeam::Anti : ETeam::Pro;
	OnRep_OccupationWinner();
}

void AOccupationGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (const auto BasePlayerState = Cast<ALakayaBasePlayerState>(PlayerState))
	{
		UpdatePlayerByTeamMap(BasePlayerState->GetTeam(), BasePlayerState);
		BasePlayerState->OnTeamChanged.AddUObject(this, &AOccupationGameState::UpdatePlayerByTeamMap, BasePlayerState);
	}
}

void AOccupationGameState::OnRep_AntiTeamScore() const
{
	OnTeamScoreSignature.Broadcast(ETeam::Anti, AntiTeamScore);
}

void AOccupationGameState::OnRep_ProTeamScore() const
{
	OnTeamScoreSignature.Broadcast(ETeam::Pro, ProTeamScore);
}

void AOccupationGameState::OnRep_OccupationWinner() const
{
	OnChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}

void AOccupationGameState::UpdatePlayerByMinimap(const ETeam& Team, ALakayaBasePlayerState* PlayerState)
{
	for (auto& Players : PlayersByTeamMap[Team])
	{
		const bool bMyPlayer = (Players == PlayerState);
		TabMinimapWidget->SetPlayersByMinimap(Team, Players, TabMinimapWidget->CreatePlayerImage(Team, bMyPlayer));
		HUDMinimapWidget->SetPlayersByMinimap(Team, Players, HUDMinimapWidget->CreatePlayerImage(Team, bMyPlayer));
	}
}

void AOccupationGameState::SetClientTeam(const ETeam& NewTeam)
{
	if (ClientTeam == NewTeam) return;
	ClientTeam = NewTeam;
	if (SpawnOutlineManager()) OutlineManager->SetTeam(NewTeam);
	for (const auto& Pair : PlayersByTeamMap)
		for (const auto& Player : Pair.Value)
			SetupPlayerStateOnLocal(Player);

	if (UOccupationCharacterSelectWidget* const OccupationCharacterSelectWidget
		= Cast<UOccupationCharacterSelectWidget>(CharacterSelectWidget))
		OccupationCharacterSelectWidget->SetTeam(NewTeam);

	if (PlayerNameDisplayerWidget.IsValid())
		PlayerNameDisplayerWidget->SetTeam(NewTeam);
}

bool AOccupationGameState::TrySendMatchResultData()
{
	if (const auto EOSGameInstance = Cast<UEOSGameInstance>(GetGameInstance()))
	{
		if (!EOSGameInstance->IsSocketConnected())
		{
			static bool DoOnce = false;
			if (!DoOnce)
			{
				EOSGameInstance->Connect();
				DoOnce = true;
			}
			return false;
		}

		return EOSGameInstance->SendMatchResultData(MatchResult);
	}
	return false;
}

bool AOccupationGameState::CanInstigatorClairvoyance(const AActor* InInstigator) const
{
	if (Super::CanInstigatorClairvoyance(InInstigator))
	{
		const auto PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController && PlayerController->IsLocalController())
		{
			// Find Instigator PlayerState
			auto InstigatorState = Cast<APlayerState>(InInstigator);
			if (!InstigatorState)
			{
				if (const auto Pawn = Cast<APawn>(InInstigator))
				{
					InstigatorState = Pawn->GetPlayerState<APlayerState>();
				}
			}

			return IsSameTeam(PlayerController->PlayerState, InstigatorState);
		}
	}

	return false;
}

bool AOccupationGameState::ShouldActivateClairvoyance() const
{
	return Super::ShouldActivateClairvoyance() && !ClairvoyanceInstigatorSet.IsEmpty();
}

void AOccupationGameState::OnClairvoyanceActivateRequested(const AActor* InInstigator)
{
	Super::OnClairvoyanceActivateRequested(InInstigator);
	ClairvoyanceInstigatorSet.Emplace(InInstigator);
}

void AOccupationGameState::OnClairvoyanceDeactivateRequested(const AActor* InInstigator)
{
	Super::OnClairvoyanceDeactivateRequested(InInstigator);
	ClairvoyanceInstigatorSet.Remove(InInstigator);
}

void AOccupationGameState::DestroyShieldWallObject()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null."));
		return;
	}

	// TArray<AActor*> FoundActors;
	TArray<TObjectPtr<AActor>> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AShieldWallObject::StaticClass(), FoundActors);

	for (const auto& ShieldActor : FoundActors)
		ShieldActor->Destroy();
}

void AOccupationGameState::UpdatePlayerByTeamMap(const ETeam& Team, ALakayaBasePlayerState* PlayerState)
{
	if (!PlayersByTeamMap.Contains(Team)) return;
	auto& PlayerStates = PlayersByTeamMap[Team];
	PlayerStates.Emplace(PlayerState);

	if (ClientTeam != ETeam::None) SetupPlayerStateOnLocal(PlayerState);

	OnPlayerStateOwnerChanged(PlayerState->GetOwner());
	PlayerState->OnOwnerChanged.AddUObject(this, &AOccupationGameState::OnPlayerStateOwnerChanged);
}

ERendererStencilMask AOccupationGameState::GetUniqueStencilMask(const bool& IsAlly, const uint8& Index)
{
	switch (Index)
	{
	case 0: return IsAlly ? ERendererStencilMask::ERSM_1 : ERendererStencilMask::ERSM_8;
	case 1: return IsAlly ? ERendererStencilMask::ERSM_2 : ERendererStencilMask::ERSM_16;
	case 2: return IsAlly ? ERendererStencilMask::ERSM_4 : ERendererStencilMask::ERSM_32;
	default: return ERendererStencilMask::ERSM_Default;
	}
}

void AOccupationGameState::OnPlayerStateOwnerChanged(AActor* InOwner)
{
	if (const auto Controller = Cast<APlayerController>(InOwner); Controller && Controller->IsLocalController())
		SetClientTeam(Controller->GetPlayerState<ALakayaBasePlayerState>()->GetTeam());
}

void AOccupationGameState::AddPlayerStateToRecordResult(ETeam InTeam, TArray<ALakayaBasePlayerState*> InPlayers)
{
	if (InTeam == ETeam::Anti)
	{
		for (const auto BasePlayerState : InPlayers)
		{
			MatchResult.AntiPlayers.Emplace(BasePlayerState->GetPlayerStats());
		}
	}
	else if (InTeam == ETeam::Pro)
	{
		for (const auto BasePlayerState : InPlayers)
		{
			MatchResult.ProPlayers.Emplace(BasePlayerState->GetPlayerStats());
		}
	}
}

void AOccupationGameState::SetScoreBoardVisibility(const bool& Visible)
{
	Super::SetScoreBoardVisibility(Visible);

	if (MatchState == MatchState::InProgress)
	{
		InternalSetScoreBoardVisibility(Visible);
	}
}

void AOccupationGameState::SetTabMinimapVisibility(const bool& Visible)
{
	Super::SetTabMinimapVisibility(Visible);

	if (MatchState == MatchState::InProgress)
	{
		InternalSetTabMinimapVisibility(Visible);
	}
}

void AOccupationGameState::InternalSetScoreBoardVisibility(const bool& Visible) const
{
	if (!ScoreBoard.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_ScoreBoard is null."));
		return;
	}

	ScoreBoard->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void AOccupationGameState::InternalSetTabMinimapVisibility(const bool& Visible) const
{
	if (!TabMinimapWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_OccupationTabMinimapWidget is null."));
		return;
	}

	TabMinimapWidget->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void AOccupationGameState::AddCaptureAreaCount(const ETeam& Team, const uint8 Id)
{
	(Team == ETeam::Anti) ? ++AntiTeamCaptureAreaCount : ++ProTeamCaptureAreaCount;
	UpdateOccupyExpressWidget(Team, Id);
}

void AOccupationGameState::SubCaptureAreaCount(const ETeam& Team)
{
	(Team == ETeam::Anti) ? --AntiTeamCaptureAreaCount : --ProTeamCaptureAreaCount;
}

void AOccupationGameState::UpdateTeamScoreTick()
{
	if (TeamToUpdate == ETeam::Anti)
	{
		AntiTeamScore += AdditiveScore;
		OnRep_AntiTeamScore();
	}
	else if (TeamToUpdate == ETeam::Pro)
	{
		ProTeamScore += AdditiveScore;
		OnRep_ProTeamScore();
	}
}

void AOccupationGameState::StartScoreUpdate(const ETeam& Team, float UpdateDelay)
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_UpdateScoreTimer))
	{
		StopScoreUpdate();
	}

	TeamToUpdate = Team;
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateScoreTimer, this, &AOccupationGameState::UpdateTeamScoreTick,
	                                UpdateDelay, true);
}

void AOccupationGameState::StopScoreUpdate()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_UpdateScoreTimer);
	TeamToUpdate = ETeam::None;
}

void AOccupationGameState::UpdateOccupyExpressWidget(const ETeam& Team, const uint8& Id)
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		LocalController && LocalController->IsLocalController())
	{
		if (UProgressBar** ProgressBar = OccupyBarMaps.Find(Id))
		{
			FSlateBrush BackGroundImageBrush;

			switch (Team)
			{
			case ETeam::Anti:
				BackGroundImageBrush.SetResourceObject(OccupyExpressWidget->GetOccupyAntiImage());
				break;
			case ETeam::Pro:
				BackGroundImageBrush.SetResourceObject(OccupyExpressWidget->GetOccupyProImage());
				break;
			default:
				UE_LOG(LogTemp, Warning, TEXT("Invalid Team."));
				return;
			}

			// ReSharper disable once CppDeprecatedEntity
			(*ProgressBar)->WidgetStyle.BackgroundImage = BackGroundImageBrush;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ProgressBar is not Found for Id : %d"), Id);
		}
	}
}

void AOccupationGameState::OnEnemySpotted(const ETeam& EnemyTeam, ALakayaBasePlayerState* Enemy)
{
	// 같은 팀의 정보는 필요 없으므로, 리턴합니다.
	if (EnemyTeam == ClientTeam) return;
	Multicast_UpdateMinimap(EnemyTeam, Enemy);
}

void AOccupationGameState::OnEnemyLost(const ETeam& EnemyTeam, ALakayaBasePlayerState* Enemy)
{
	// 같은 팀의 정보는 필요 없으므로, 리턴합니다.
	if (EnemyTeam == ClientTeam) return;
	MultiCast_HideFromMinimap(EnemyTeam, Enemy);
}

void AOccupationGameState::MultiCast_HideFromMinimap_Implementation(const ETeam& EnemyTeam, ALakayaBasePlayerState* Enemy)
{
	if (TabMinimapWidget && HUDMinimapWidget)
	{
		// TabMinimapWidget->HidePlayerPosition(EnemyTeam, Enemy);
		// HUDMinimapWidget->HidePlayerPosition(EnemyTeam, Enemy);
	}
}

void AOccupationGameState::Multicast_UpdateMinimap_Implementation(const ETeam& EnemyTeam,
                                                                  ALakayaBasePlayerState* Enemy)
{
	if (TabMinimapWidget && HUDMinimapWidget)
	{
		TabMinimapWidget->UpdatePlayerPosition(EnemyTeam, Enemy);
		HUDMinimapWidget->UpdatePlayerPosition(EnemyTeam, Enemy);
	}
}

bool AOccupationGameState::CheckCaptureAreaCount(const ETeam& Team)
{
	const int AntiCaptureAreaCount = GetAntiTeamCaptureAreaCount();
	const int ProCaptureAreaCount = GetProTeamCaptureAreaCount();

	if (AntiCaptureAreaCount == ProCaptureAreaCount)
	{
		StopScoreUpdate();
		return false;
	}

	return (Team == ETeam::Anti)
		       ? (AntiCaptureAreaCount > ProCaptureAreaCount)
		       : (AntiCaptureAreaCount < ProCaptureAreaCount);
}

void AOccupationGameState::SetupPlayerStateOnLocal(ALakayaBasePlayerState* PlayerState)
{
	const auto Team = PlayerState->GetTeam();
	const auto IsAlly = Team == ClientTeam;
	PlayerState->SetUniqueStencilMask(GetUniqueStencilMask(IsAlly, PlayersByTeamMap[Team].Find(PlayerState)));
	PlayerState->SetAlly(IsAlly);
	if (const auto Widget = GetCharacterSelectWidget(); Widget && IsAlly)
		Widget->RegisterPlayer(PlayerState);
}
