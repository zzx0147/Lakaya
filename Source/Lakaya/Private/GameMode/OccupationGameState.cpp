#include "GameMode/OccupationGameState.h"

#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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
#include "UI/IntroWidget.h"
#include "UI/MatchStartWaitWidget.h"
#include "UI/OccupationCharacterSelectWidget.h"
#include "UI/OccupationOverlayMinimapWidget.h"
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

	CaptureOwnerMap.Emplace(0, ETeam::None);
	CaptureOwnerMap.Emplace(1, ETeam::None);
	CaptureOwnerMap.Emplace(2, ETeam::None);
	
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ResultContextFinder(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IC_ResultWidgetControl.IC_ResultWidgetControl'"));

	static ConstructorHelpers::FObjectFinder<UInputAction> ResultSwitchActionFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ShowScore.IA_ShowScore'"));

	ResultShortcutContext = ResultContextFinder.Object;
	ResultSwitchingAction = ResultSwitchActionFinder.Object;

	HUDMinimapWidgetClass = UOccupationOverlayMinimapWidget::StaticClass();
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
				GameResultWidget->AddToViewport(20);
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
				OccupyBarMaps.Emplace(1, OccupyExpressWidget->GetAntiAreaBar());
				OccupyExpressWidget->GetAntiAreaBar()->InitOccupyExpressElementWidget(1, OccupyExpressWidget->GetAntiAreaBar()->GetAntiAreaNoneImage());
				OccupyExpressWidget->GetAntiAreaBar()->GetProgressBar()->SetPercent(0);
				OccupyBarMaps.Emplace(2, OccupyExpressWidget->GetCenterAreaBar());
				OccupyExpressWidget->GetCenterAreaBar()->InitOccupyExpressElementWidget(2, OccupyExpressWidget->GetCenterAreaBar()->GetCenterAreaNoneImage());
				OccupyExpressWidget->GetCenterAreaBar()->GetProgressBar()->SetPercent(0);
				OccupyBarMaps.Emplace(3, OccupyExpressWidget->GetProAreaBar());
				OccupyExpressWidget->GetProAreaBar()->InitOccupyExpressElementWidget(3, OccupyExpressWidget->GetProAreaBar()->GetProAreaNoneImage());
				OccupyExpressWidget->GetProAreaBar()->GetProgressBar()->SetPercent(0);
			}
		}

		if (HUDMinimapWidgetClass)
		{
			HUDMinimapWidget = CreateWidget<UOccupationOverlayMinimapWidget>(
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

		LocalController->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(LocalController);
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
				HUDMinimapWidget->SetEnemyTeam(LakayaPlayerState->GetTeam() == ETeam::Anti ? ETeam::Pro : ETeam::Anti);
				
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
				TabMinimapWidget->SetEnemyTeam(LakayaPlayerState->GetTeam() == ETeam::Anti ? ETeam::Pro : ETeam::Anti);
				
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

void AOccupationGameState::HandleMatchIsIntro()
{
	Super::HandleMatchIsIntro();
	
	if(!PlayersByTeamMap.Contains(ETeam::Pro) || !PlayersByTeamMap.Contains(ETeam::Anti)) return;

	if (IntroWidget) IntroWidget->SetPlayersData(PlayerArray);
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
	if(!TabMinimapWidget || !HUDMinimapWidget) return;
	
	for (auto& Players : PlayersByTeamMap[Team])
	{
		const bool bMyPlayer = (Players == PlayerState);
		TabMinimapWidget->SetOccupationPlayersByMinimap(Team, Players, TabMinimapWidget->CreatePlayerImage(Team, bMyPlayer));
		HUDMinimapWidget->SetOccupationPlayersByMinimap(Team, Players, HUDMinimapWidget->CreatePlayerImage(Team, bMyPlayer));
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

void AOccupationGameState::OnRep_MatchEndingTime()
{
	Super::OnRep_MatchEndingTime();
	if(InGameTimeWidget.IsValid()) InGameTimeWidget->SetVisibility(ESlateVisibility::Hidden);
	if(TeamScoreWidget) TeamScoreWidget->SetMaxScoreVisibility(true);
	
}

void AOccupationGameState::DestroyShieldWallObject() const
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

void AOccupationGameState::UpdateOccupyExpressWidget(const ETeam& Team, const uint8& Id) const
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		LocalController && LocalController->IsLocalController())
	{
		const TObjectPtr<UImage>* ImageWidget = nullptr;
		const TObjectPtr<UTexture2D>* ImageTexture = nullptr;

		switch (Id)
		{
		case 1:
			ImageWidget = &OccupyExpressWidget->GetAntiAreaBar()->GetInImage();
			ImageTexture = (Team == ETeam::Anti) ? &OccupyExpressWidget->GetAntiAreaBar()->GetAntiAreaAntiImage()
												 : &OccupyExpressWidget->GetAntiAreaBar()->GetAntiAreaProImage();
			break;
		case 2:
			ImageWidget = &OccupyExpressWidget->GetCenterAreaBar()->GetInImage();
			ImageTexture = (Team == ETeam::Anti) ? &OccupyExpressWidget->GetCenterAreaBar()->GetCenterAreaAntiImage()
												 : &OccupyExpressWidget->GetCenterAreaBar()->GetCenterAreaProImage();
			break;
		case 3:
			ImageWidget = &OccupyExpressWidget->GetProAreaBar()->GetInImage();
			ImageTexture = (Team == ETeam::Anti) ? &OccupyExpressWidget->GetProAreaBar()->GetProAreaAntiImage()
												 : &OccupyExpressWidget->GetProAreaBar()->GetProAreaProImage();
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("점령 ID가 존재하지 않습니다."));
			return;
		}

		if (ImageWidget != nullptr && ImageTexture != nullptr)
		{
			(*ImageWidget)->SetBrushFromTexture(*ImageTexture);
		}
	}
}

void AOccupationGameState::UpdateExpressWidget(const ETeam& Team, const uint8& Id, const float& Progress)
{
	const TObjectPtr<UOccupyExpressElementWidget>* ProgressBar = OccupyBarMaps.Find(Id);

	if (ProgressBar == nullptr || *ProgressBar == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProgressBar is not Found for Id : %d"), Id);
		return;
	}

	UOccupyExpressElementWidget* BarWidget = *ProgressBar;
	
	switch (Team)
	{
	case ETeam::Anti:
		BarWidget->GetProgressBar()->SetFillImage(OccupyExpressWidget->GetAntiAreaBar()->GetAntiFillImage());
		break;
	case ETeam::Pro:
		BarWidget->GetProgressBar()->SetFillImage(OccupyExpressWidget->GetAntiAreaBar()->GetProFillImage());
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid Team."));
		return;
	}

	BarWidget->GetProgressBar()->SetPercent(Progress / 4);
}

void AOccupationGameState::SetCaptureOwnerChange(const uint8 NewCaptureId, const ETeam& NewTeam)
{
	CaptureOwnerMap.Emplace(NewCaptureId, NewTeam);

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		LocalController && LocalController->IsLocalController())
	{
		UpdateAreaImage(NewCaptureId, NewTeam);
	}
}

void AOccupationGameState::UpdateAreaImage(const uint8 NewCaptureId, const ETeam& NewTeam) const
{
	UTexture2D* AntiImage = nullptr;
	UTexture2D* ProImage = nullptr;
	switch (NewCaptureId)
	{
	case 1:
		AntiImage = HUDMinimapWidget->GetAntiAreaAntiImage();
		ProImage = HUDMinimapWidget->GetAntiAreaProImage();
		break;
	case 2:
		AntiImage = HUDMinimapWidget->GetCenterAreaAntiImage();
		ProImage = HUDMinimapWidget->GetCenterAreaProImage();
		break;
	case 3:
		AntiImage = HUDMinimapWidget->GetProAreaAntiImage();
		ProImage = HUDMinimapWidget->GetProAreaProImage();
		break;
	default:
		return;
	}

	UTexture2D* NewTexture = (NewTeam == ETeam::Anti) ? AntiImage : ProImage;
	UpdateImage(NewCaptureId, NewTexture);
}

void AOccupationGameState::UpdateImage(const uint8 NewCaptureId, UTexture2D* NewTexture) const
{
	switch (NewCaptureId)
	{
	case 1:
		HUDMinimapWidget->GetAntiAreaImage()->SetBrushFromTexture(NewTexture);
		TabMinimapWidget->GetAntiAreaImage()->SetBrushFromTexture(NewTexture);
		break;
	case 2:
		HUDMinimapWidget->GetCenterAreaImage()->SetBrushFromTexture(NewTexture);
		TabMinimapWidget->GetCenterAreaImage()->SetBrushFromTexture(NewTexture);
		break;
	case 3:
		HUDMinimapWidget->GetProAreaImage()->SetBrushFromTexture(NewTexture);
		TabMinimapWidget->GetProAreaImage()->SetBrushFromTexture(NewTexture);
		break;
	default: ;
	}
}

void AOccupationGameState::OnEnemySpotted(const ETeam& EnemyTeam, ALakayaBasePlayerState* Enemy)
{
	// 같은 팀의 정보는 필요 없으므로, 리턴합니다.
	// if (EnemyTeam == ClientTeam) return;
	Multicast_UpdateMinimap(EnemyTeam, Enemy);
}

void AOccupationGameState::OnEnemyLost(const ETeam& EnemyTeam, ALakayaBasePlayerState* Enemy)
{
	// 같은 팀의 정보는 필요 없으므로, 리턴합니다.
	// if (EnemyTeam == ClientTeam) return;
	MultiCast_HideFromMinimap(EnemyTeam, Enemy);
}

TArray<ALakayaBasePlayerState*> AOccupationGameState::GetAllyArray(UObject* TeamObject) const
{
	if (const auto CastedObject = Cast<ITeamObjectInterface>(TeamObject))
	{
		if (const auto Found = PlayersByTeamMap.Find(CastedObject->GetTeam()))
		{
			return *Found;
		}
	}
	return {};
}

TArray<ALakayaBasePlayerState*> AOccupationGameState::GetEnemyArray(UObject* TeamObject) const
{
	if (const auto CastedObject = Cast<ITeamObjectInterface>(TeamObject))
	{
		switch (CastedObject->GetTeam())
		{
		case ETeam::Anti: return PlayersByTeamMap[ETeam::Pro];
		case ETeam::Pro: return PlayersByTeamMap[ETeam::Anti];
		default: return {};
		}
	}
	return {};
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