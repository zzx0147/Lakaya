#include "GameMode/OccupationGameState.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "ETC/OutlineManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "GameMode/OccupationGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Occupation/ShieldWallObject.h"
#include "PlayerController/BattlePlayerController.h"
#include "UI/DetailResultElementWidget.h"
#include "UI/DetailResultWidget.h"
#include "UI/FinalResultWidget.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GameResultWidget.h"
#include "UI/GradeResultElementWidget.h"
#include "UI/GradeResultWidget.h"
#include "UI/MatchStartWaitWidget.h"
#include "UI/OccupationCharacterSelectWidget.h"
#include "UI/OccupationMinimapWidget.h"
#include "UI/StartMessageWidget.h"
#include "UI/TeamScoreWidget.h"
#include "UI/WeaponOutLineWidget.h"
#include "UI/OccupyExpressWidget.h"

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

AOccupationGameState::AOccupationGameState()
{
	MaxScore = 100.f;
	ScoreUpdateDelay = 1.0f;
	AdditiveScore = 3.0f;
	MatchDuration = 180.f;
	MatchStartWaitWidgetLifeTime = 3.0f;
	MatchStartWidgetLifeTime = 5.0f;
	ClientTeam = ETeam::None;

	PlayersByTeamMap.Emplace(ETeam::Anti);
	PlayersByTeamMap.Emplace(ETeam::Pro);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ResultContextFinder(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IC_ResultWidgetControl.IC_ResultWidgetControl'"));

	static ConstructorHelpers::FObjectFinder<UInputAction> ResultSwitchActionFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ShowScore.IA_ShowScore'"));

	ResultShortcutContext = ResultContextFinder.Object;
	ResultSwitchingAction = ResultSwitchActionFinder.Object;

	bTap = true;
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

		if (GradeResultWidgetClass)
		{
			GradeResultWidget = CreateWidget<UGradeResultWidget>(LocalController, GradeResultWidgetClass);
			if (GradeResultWidget.IsValid())
			{
				GradeResultWidget->AddToViewport(1);
				GradeResultWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (GradeResultElementWidgetClass)
		{
			GradeResultElementWidget = CreateWidget<UGradeResultElementWidget>(
				LocalController, GradeResultElementWidgetClass);
			if (GradeResultElementWidget.IsValid())
			{
				GradeResultElementWidget->AddToViewport(1);
				GradeResultElementWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (DetailResultWidgetClass)
		{
			DetailResultWidget = CreateWidget<UDetailResultWidget>(LocalController, DetailResultWidgetClass);
			if (DetailResultWidget.IsValid())
			{
				DetailResultWidget->AddToViewport(1);
				DetailResultWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (DetailResultElementWidgetClass)
		{
			DetailResultElementWidget = CreateWidget<UDetailResultElementWidget>(
				LocalController, DetailResultElementWidgetClass);
			if (DetailResultElementWidget.IsValid())
			{
				DetailResultElementWidget->AddToViewport(1);
				DetailResultElementWidget->SetVisibility(ESlateVisibility::Hidden);
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

		if (OccupationMinimapWidgetClass)
		{
			OccupationMinimapWidget = CreateWidget<UOccupationMinimapWidget>(
				LocalController, OccupationMinimapWidgetClass);
			if (OccupationMinimapWidget.IsValid())
			{
				OccupationMinimapWidget->AddToViewport();
				OccupationMinimapWidget->SetVisibility(ESlateVisibility::Hidden);
				OccupationMinimapWidget->PlayersByMinimap.Emplace(ETeam::Anti);
				OccupationMinimapWidget->PlayersByMinimap.Emplace(ETeam::Pro);

				
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

	if (OccupationMinimapWidget.IsValid())
	{
		OccupationMinimapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OccupationMinimapWidget->UpdateMinimap = true;
		OccupationMinimapWidget->PlayersByMinimap.Emplace(ETeam::Anti);
		OccupationMinimapWidget->PlayersByMinimap.Emplace(ETeam::Pro);

		if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		LocalController && LocalController->IsLocalController())
		{
			if (const auto LakayaPlayerState = LocalController->GetPlayerState<ALakayaBasePlayerState>())
			{
				OccupationMinimapWidget->SetTeam(LakayaPlayerState->GetTeam());

				// 와지가 스킬을 쓰게 되면 상대방의 위치도 미니맵 상에 업데이트 해줘야 하기 때문에
				// 일단은 본인의 팀과 상대방의 팀의 정보를 미니맵 위젯에 넣어주도록 합니다.
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
	GetWorldTimerManager().SetTimer(TimerHandle_MatchStartWaitWidget, TimerDelegate_MatchStartWaitWidget, MatchWaitDuration - 10,false);
	
	// MatchStartWaitWidget위젯을 띄우고, N초(MatchStartWaitWidgetLifeTime) 뒤에 비활성화 해줍니다.
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]
	{
		if (this == nullptr) return;
		if (MatchStartWaitWidget.IsValid()) MatchStartWaitWidget->SetVisibility(ESlateVisibility::Hidden);
	});
	GetWorldTimerManager().SetTimer(TimerHandle_WaitTimerHandle, TimerDelegate,   (MatchWaitDuration - 10 + MatchStartWaitWidgetLifeTime), false);

	// 게임이 본격적으로 시작이 되면 StartMessage위젯을 띄워줍니다.
	TimerDelegate.BindLambda([this]
	{
		if (this == nullptr) return;
		if (StartMessageWidget.IsValid()) StartMessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		DestroyShieldWallObject();
	});
	GetWorldTimerManager().SetTimer(TimerHandle_StartMessageVisible, TimerDelegate, MatchWaitDuration, false);

	// StartMessage위젯을 5초 뒤에 비활성화 해줍니다.
	TimerDelegate.BindLambda([this]
	{
		if (this == nullptr) return;
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

	bTap = false;

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
	LocalController && LocalController->IsLocalController())
	{
		FinalResultWidget->SetTeam(ClientTeam);
		FinalResultWidget->SetMatchResultData(GetOccupationWinner(),ProTeamScore,AntiTeamScore, PlayersByTeamMap);
	
		ShowEndResultWidget();
		BindDetailResultWidget();
		BindDetailResultElementWidget();
	}

	if (OccupationMinimapWidget.IsValid())
		OccupationMinimapWidget->UpdateMinimap = false;
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

void AOccupationGameState::ChangeResultWidget()
{
	if (!GradeResultWidget.IsValid() || !DetailResultWidget.IsValid() || !DetailResultElementWidget.IsValid()) return;
	ESlateVisibility gradeVisibility = ResultBool ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible;
	ESlateVisibility detailVisibility = ResultBool ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;

	GradeResultWidget->SetVisibility(gradeVisibility);
	DetailResultWidget->SetVisibility(detailVisibility);
	DetailResultElementWidget->SetVisibility(detailVisibility);

	ResultBool = !ResultBool;
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

void AOccupationGameState::OnRep_AntiTeamScore()
{
	OnTeamScoreSignature.Broadcast(ETeam::Anti, AntiTeamScore);
}

void AOccupationGameState::OnRep_ProTeamScore()
{
	OnTeamScoreSignature.Broadcast(ETeam::Pro, ProTeamScore);
}

void AOccupationGameState::OnRep_OccupationWinner()
{
	OnChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}

void AOccupationGameState::UpdatePlayerByMinimap(const ETeam& Team, ALakayaBasePlayerState* PlayerState)
{
	for (auto& Players : PlayersByTeamMap[Team])
	{
		const bool bMyPlayer = (Players == PlayerState);
		OccupationMinimapWidget->PlayersByMinimap[Team].Emplace(Players, OccupationMinimapWidget->CreatePlayerImage(Team, bMyPlayer));
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

	if(UOccupationCharacterSelectWidget* const OccupationCharacterSelectWidget = Cast<UOccupationCharacterSelectWidget>(
		CharacterSelectWidget))
	{
		OccupationCharacterSelectWidget->SetTeam(NewTeam);
	}
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

void AOccupationGameState::ShowEndResultWidget()
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		GameResultWidget.IsValid() && LocalController && LocalController->IsLocalController())
	{
		LocalController->SetShowMouseCursor(false);
		LocalController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_None);
		const auto LobbyController = Cast<AGameLobbyPlayerController>(LocalController);
		LobbyController->UnbindAllAndBindMenu(Cast<UEnhancedInputComponent>(LocalController->InputComponent));

		if (const auto LakayaPlayerState = LocalController->GetPlayerState<ALakayaBasePlayerState>())
		{
			if (LakayaPlayerState->IsSameTeam(GetOccupationWinner()))
			{
				// 승리했다면 "승리" 이미지를 띄워줍니다.
				GameResultWidget->VictoryImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				// 패배했다면 "패배" 이미지를 띄워줍니다.
				GameResultWidget->DefeatImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}

			GameResultWidget->AntiScore->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), AntiTeamScore)));
			GameResultWidget->ProScore->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), ProTeamScore)));

			ShowGradeResultWidget(LakayaPlayerState, LocalController);
		}
		else UE_LOG(LogTemp, Warning, TEXT("LakayaPlayerState is null."));
	}
}

void AOccupationGameState::ShowGradeResultWidget(ALakayaBasePlayerState* PlayerState, APlayerController* Controller)
{
	// 게임 결과를 띄우고 5초뒤에 비활성화 해주고, GradeResultWidget, GradeResultElementWidget을 띄웁니다.
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, PlayerState, Controller]
	{
		if (this == nullptr || !GameResultWidget.IsValid() || !GradeResultWidget.IsValid()) return;
		GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
		GradeResultWidget->SetVisibility(ESlateVisibility::Hidden);
		FinalResultWidget->SetVisibility(ESlateVisibility::Visible);
		
		if (PlayerState->GetTeam() == ETeam::Anti)
			GradeResultWidget->AntiTextBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (PlayerState->GetTeam() == ETeam::Pro)
			GradeResultWidget->ProTextBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (PlayerState->IsSameTeam(GetOccupationWinner()))
			GradeResultWidget->VictoryImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		else GradeResultWidget->DefeatImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		Controller->SetShowMouseCursor(false);

		if (Controller != nullptr && Controller->GetLocalPlayer() != nullptr)
		{
			if (const auto SubSystem = Controller->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				SubSystem->AddMappingContext(ResultShortcutContext, 200);
			}
		}

		ShowGradeResultElementWidget(PlayerState);
		bTap = true;
	});
	GetWorldTimerManager().SetTimer(TimerHandle_GameResultHandle, TimerDelegate, 5.0f, false);
}

void AOccupationGameState::ShowGradeResultElementWidget(ALakayaBasePlayerState* NewPlayerState) const
{
	if (GradeResultElementWidget.IsValid()) GradeResultElementWidget->SetVisibility(ESlateVisibility::Hidden);

	if (NewPlayerState->GetTeam() == ETeam::Anti)
	{
		ShowAntiTeamGradeResultElementWidget();
	}

	if (NewPlayerState->GetTeam() == ETeam::Pro)
	{
		ShowProTeamGradeResultElementWidget();
	}
}

void AOccupationGameState::GradeResultTeamInfo(TArray<TObjectPtr<ALakayaBasePlayerState>>& NewPlayerArray,
                                               uint8 NewIndex) const
{
	if (!GradeResultElementWidget.IsValid()) return;
	FString CharacterName;
	if (NewPlayerArray[NewIndex]->GetCharacterName() == "Rena")
		CharacterName.Append("Rena");
	if (NewPlayerArray[NewIndex]->GetCharacterName() == "Wazi")
		CharacterName.Append("Wazi");

	FString RankLetter;
	switch (NewIndex)
	{
	case 0:
		RankLetter = "First";
		break;
	case 1:
		RankLetter = "Second";
		break;
	case 2:
		RankLetter = "Third";
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("존재하는 않은 Index입니다."));
		break;
	}

	FString TeamLetter;
	if (NewPlayerArray[NewIndex]->GetTeam() == ETeam::Anti)
		TeamLetter = "Anti";
	if (NewPlayerArray[NewIndex]->GetTeam() == ETeam::Pro)
		TeamLetter = "Pro";

	GradeResultElementWidget->GetWidgetFromName(
		*FString::Printf(TEXT("%s_%s_%s_Image"), *RankLetter, *TeamLetter, *CharacterName))->SetVisibility(
		ESlateVisibility::SelfHitTestInvisible);
	GradeResultElementWidget->GetWidgetFromName(
		*FString::Printf(TEXT("%s_%s_RankBoard_Image"), *RankLetter, *TeamLetter))->SetVisibility(
		ESlateVisibility::SelfHitTestInvisible);

	FString FormattedName = FString::Printf(TEXT("%s"), *NewPlayerArray[NewIndex]->GetPlayerName());
	UTextBlock* NameText = Cast<UTextBlock>(
		GradeResultElementWidget->GetWidgetFromName(*FString::Printf(TEXT("%s_Name_Text"), *RankLetter)));
	NameText->SetText(FText::FromString(FormattedName));
	NameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	UTextBlock* TotalScoreText = Cast<UTextBlock>(
		GradeResultElementWidget->GetWidgetFromName(*FString::Printf(TEXT("%s_TotalScore_Text"), *RankLetter)));
	TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), NewPlayerArray[NewIndex]->GetTotalScore())));
	TotalScoreText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	GradeResultElementWidget->GetWidgetFromName(*FString::Printf(TEXT("%s_Rank_Text"), *RankLetter))->SetVisibility(
		ESlateVisibility::SelfHitTestInvisible);
	GradeResultElementWidget->GetWidgetFromName(*FString::Printf(TEXT("%s_ScoreBoard_Icon_Image"), *RankLetter))->
	                          SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void AOccupationGameState::ShowAntiTeamGradeResultElementWidget() const
{
	TArray<TObjectPtr<ALakayaBasePlayerState>> AntiPlayerArray;
	int index = 0;
	for (auto RegisterPlayerState : PlayersByTeamMap[ETeam::Anti])
	{
		AntiPlayerArray.Add(RegisterPlayerState);
		GradeResultTeamInfo(AntiPlayerArray, index);
		++index;
	}
}

void AOccupationGameState::ShowProTeamGradeResultElementWidget() const
{
	TArray<TObjectPtr<ALakayaBasePlayerState>> ProPlayerArray;
	int index = 0;
	for (auto RegisterPlayerState : PlayersByTeamMap[ETeam::Pro])
	{
		ProPlayerArray.Add(RegisterPlayerState);
		GradeResultTeamInfo(ProPlayerArray, index);
		++index;
	}
}

void AOccupationGameState::BindDetailResultWidget()
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController();
		DetailResultWidget.IsValid() && LocalController && LocalController->IsLocalController())
	{
		const ALakayaBasePlayerState* PlayerState = Cast<ALakayaBasePlayerState>(LocalController->PlayerState);

		// Anti
		if (PlayerState->GetTeam() == ETeam::Anti)
		{
			DetailResultWidget->AntiTextBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			DetailResultWidget->AntiUserBoxNameImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			DetailResultWidget->AntiUserBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			DetailResultWidget->AntiTotalBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (PlayerState->GetCharacterName() == "Rena")
			{
				DetailResultWidget->AntiRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->PortraitRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->UserBoxCharacterNameText->SetText(FText::FromString(TEXT("RENA")));
			}

			if (PlayerState->GetCharacterName() == "Wazi")
			{
				DetailResultWidget->AntiWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->PortraitWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->UserBoxCharacterNameText->SetText(FText::FromString(TEXT("WAZI")));
			}
		}

		// Pro
		if (PlayerState->GetTeam() == ETeam::Pro)
		{
			DetailResultWidget->ProTextBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			DetailResultWidget->ProUserBoxNameImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			DetailResultWidget->ProUserBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			DetailResultWidget->ProTotalBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (PlayerState->GetCharacterName() == "Rena")
			{
				DetailResultWidget->ProRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->PortraitRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->UserBoxCharacterNameText->SetText(FText::FromString(TEXT("RENA")));
			}

			if (PlayerState->GetCharacterName() == "Wazi")
			{
				DetailResultWidget->ProWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->PortraitWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->UserBoxCharacterNameText->SetText(FText::FromString(TEXT("WAZI")));
			}
		}

		// 승리
		if (PlayerState->IsSameTeam(GetOccupationWinner()))
		{
			DetailResultWidget->VictoryImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		// 패배
		if (!PlayerState->IsSameTeam(GetOccupationWinner()))
		{
			DetailResultWidget->DefeatImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		FString FormattedName;
		FormattedName = FString::Printf(TEXT("%s"), *PlayerState->GetPlayerName());
		DetailResultWidget->UserBoxNameText->SetText(FText::FromString(FormattedName));

		DetailResultWidget->InfoBoxScoreText->SetText(
			FText::FromString(FString::Printf(TEXT("%d"), PlayerState->GetTotalScore())));
		DetailResultWidget->InfoBoxOccupationText->SetText(
			FText::FromString(FString::Printf(TEXT("%d"), PlayerState->GetSuccessCaptureCount())));
		DetailResultWidget->InfoBoxKillText->SetText(
			FText::FromString(FString::Printf(TEXT("%d"), PlayerState->GetKillCount())));
		DetailResultWidget->InfoBoxDeathText->SetText(
			FText::FromString(FString::Printf(TEXT("%d"), PlayerState->GetDeathCount())));
	}
}

void AOccupationGameState::BindDetailResultElementWidget()
{
	if (!DetailResultElementWidget.IsValid()) return;
	uint8 AntiIndex = 0;
	uint8 ProIndex = 0;

	for (auto& ElementPlayerState : PlayersByTeamMap[ETeam::Anti])
	{
		DetailResultElementWidget->SetElementWidget(ElementPlayerState, AntiIndex);
		++AntiIndex;
	}

	for (auto& ElementPlayerState : PlayersByTeamMap[ETeam::Pro])
	{
		DetailResultElementWidget->SetElementWidget(ElementPlayerState, ProIndex);
		++ProIndex;
	}
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
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateScoreTimer, this, &AOccupationGameState::UpdateTeamScoreTick, UpdateDelay, true);
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

bool AOccupationGameState::CheckCaptureAreaCount(const ETeam& Team)
{
	int AntiCaptureAreaCount = GetAntiTeamCaptureAreaCount();
	int ProCaptureAreaCount = GetProTeamCaptureAreaCount();

	if (AntiCaptureAreaCount == ProCaptureAreaCount)
	{
		StopScoreUpdate();
		return false;
	}
	
	return (Team == ETeam::Anti) ? (AntiCaptureAreaCount > ProCaptureAreaCount) : (AntiCaptureAreaCount < ProCaptureAreaCount);
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