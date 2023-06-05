#include "GameMode/OccupationGameState.h"

#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Engine/TriggerBox.h"
#include "ETC/OutlineManager.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "GameMode/OccupationGameMode.h"
#include "Net/UnrealNetwork.h"
#include "UI/DetailResultWidget.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GameResultWidget.h"
#include "UI/GradeResultElementWidget.h"
#include "UI/GradeResultWidget.h"
#include "UI/MatchStartWaitWidget.h"
#include "UI/StartMessageWidget.h"
#include "UI/TeamScoreWidget.h"

void AOccupationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);
	DOREPLIFETIME(AOccupationGameState, CurrentOccupationWinner);
}

AOccupationGameState::AOccupationGameState()
{
	MaxScore = 100.f;
	MatchDuration = 180.f;
	MatchStartWaitWidgetLifeTime = 3.0f;
	MatchStartWidgetLifeTime = 5.0f;
	ClientTeam = EPlayerTeam::None;

	PlayersByTeamMap.Emplace(EPlayerTeam::A);
	PlayersByTeamMap.Emplace(EPlayerTeam::B);
}

void AOccupationGameState::BeginPlay()
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		LocalController && LocalController->IsLocalController())
	{
		if (TeamScoreWidgetClass)
		{
			TeamScoreWidget = CreateWidget<UTeamScoreWidget>(LocalController, TeamScoreWidgetClass);
			if (TeamScoreWidget == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("TeamScoreWidget is null."));
			}
			TeamScoreWidget->AddToViewport();
			TeamScoreWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if (MatchStartWaitWidgetClass)
		{
			MatchStartWaitWidget = CreateWidget<UMatchStartWaitWidget>(LocalController, MatchStartWaitWidgetClass);
			if (MatchStartWaitWidget == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("MatchStartWaitWidget is null."));
			}
			MatchStartWaitWidget->AddToViewport();
			MatchStartWaitWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if (StartMessageWidgetClass)
		{
			StartMessageWidget = CreateWidget<UStartMessageWidget>(LocalController, StartMessageWidgetClass);
			if (StartMessageWidget == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("StartMessageWidget is null."));
			}
			StartMessageWidget->AddToViewport();
			StartMessageWidget->SetVisibility(ESlateVisibility::Hidden);
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_LocalPlayerController is null."));
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

	if (MatchStartWaitWidget.IsValid())
		MatchStartWaitWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// MatchStartWaitWidget위젯을 띄우고, 3초 뒤에 비활성화 해줍니다.
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]
	{
		if (MatchStartWaitWidget.IsValid()) MatchStartWaitWidget->SetVisibility(ESlateVisibility::Hidden);
	});
	GetWorldTimerManager().SetTimer(TimerHandle_WaitTimerHandle, TimerDelegate, MatchStartWaitWidgetLifeTime, false);

	// 게임이 본격적으로 시작이 되면 StartMessage위젯을 띄워줍니다.
	TimerDelegate.BindLambda([this]
	{
		if (StartMessageWidget.IsValid()) StartMessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		DestroyTriggerBox();
	});
	GetWorldTimerManager().SetTimer(TimerHandle_StartMessageVisible, TimerDelegate, MatchWaitDuration, false);

	// StartMessage위젯을 5초 뒤에 비활성화 해줍니다.
	TimerDelegate.BindLambda([this]
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
	for (auto& Element : PlayersByTeamMap)
	{
		Element.Value.Sort([](const ALakayaBasePlayerState& A, const ALakayaBasePlayerState& B)
		{
			return A.GetTotalScore() > B.GetTotalScore();
		});
	}

	ShowEndResultWidget();
	TapBool = false;
}

void AOccupationGameState::NotifyKillCharacter_Implementation(AController* KilledController, AActor* KilledActor,
                                                              AController* EventInstigator, AActor* Causer)
{
	//OnKillCharacterNotify.Broadcast(KilledController, KilledActor, EventInstigator, Causer);
}

void AOccupationGameState::EndTimeCheck()
{
	if (ATeamScore >= MaxScore || BTeamScore >= MaxScore)
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
	ESlateVisibility gradeVisibility = ResultBool ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible;
	ESlateVisibility detailVisibility = ResultBool ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;

	GradeResultWidget->SetVisibility(gradeVisibility);
	DetailResultWidget->SetVisibility(detailVisibility);

	ResultBool = !ResultBool;
}

void AOccupationGameState::SetOccupationWinner()
{
	CurrentOccupationWinner = ATeamScore > BTeamScore ? EPlayerTeam::A : EPlayerTeam::B;
	OnRep_OccupationWinner();
}

void AOccupationGameState::AddTeamScore(const EPlayerTeam& Team, const float& AdditiveScore)
{
	if (Team == EPlayerTeam::A)
	{
		ATeamScore += AdditiveScore;
		OnRep_ATeamScore();
	}
	else if (Team == EPlayerTeam::B)
	{
		BTeamScore += AdditiveScore;
		OnRep_BTeamScore();
	}
}

void AOccupationGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (const auto BasePlayerState = Cast<ALakayaBasePlayerState>(PlayerState))
	{
		auto SetupPlayerTeam = [this](const EPlayerTeam& Team, ALakayaBasePlayerState* InPlayerState)
		{
			if (!PlayersByTeamMap.Contains(Team)) return;
			PlayersByTeamMap[Team].Emplace(InPlayerState);
			if (ClientTeam == Team) CharacterSelectWidget->RegisterPlayer(InPlayerState);
		};
		
		SetupPlayerTeam(BasePlayerState->GetTeam(), BasePlayerState);
		BasePlayerState->OnTeamChanged.AddLambda(SetupPlayerTeam, BasePlayerState);
	}
}

float AOccupationGameState::GetTeamScore(const EPlayerTeam& Team) const
{
	if (Team == EPlayerTeam::A) return ATeamScore;
	if (Team == EPlayerTeam::B) return BTeamScore;
	UE_LOG(LogScript, Warning, TEXT("Trying to GetTeamScore with not valid value! it was %d"), Team);
	return 0.f;
}

void AOccupationGameState::OnRep_ATeamScore()
{
	OnTeamScoreSignature.Broadcast(EPlayerTeam::A, ATeamScore);
}

void AOccupationGameState::OnRep_BTeamScore()
{
	OnTeamScoreSignature.Broadcast(EPlayerTeam::B, BTeamScore);
}

void AOccupationGameState::OnRep_OccupationWinner()
{
	OnChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}

void AOccupationGameState::SetClientTeam(const EPlayerTeam& NewTeam)
{
	ClientTeam = NewTeam;
	if (SpawnOutlineManager()) OutlineManager->SetTeam(NewTeam);
}

void AOccupationGameState::CreateCharacterSelectWidget(APlayerController* LocalController)
{
	Super::CreateCharacterSelectWidget(LocalController);

	if (const auto BasePlayerState = LocalController->GetPlayerState<ALakayaBasePlayerState>())
	{
		const auto NewTeam = BasePlayerState->GetTeam();
		if (NewTeam == EPlayerTeam::A || NewTeam == EPlayerTeam::B)
		{
			ClientTeam = NewTeam; //로컬 컨트롤러의 팀을 저장한다.(로컬 컨트롤러의 팀에 따라 표기할 팀이 달라짐)

			//TODO: 이렇게 하기보다는 PlayersByTeamMap.Contains(PlayerTeam)을 사용하는 게 좋습니다.
			for (const auto temp : PlayersByTeamMap[ClientTeam])
			{
				CharacterSelectWidget->RegisterPlayer(temp);
				//현재까지 등록된 플레이어 스테이트들을 위젯에 등록한다
			}
		}
		//TODO: 팀이 없는 경우에만 람다를 등록하기보다는, 팀이 있더라도 게임도중 팀이 변경되는 경우를 상정하여 그냥 람다를 등록하는 것이 좋을 것 같습니다.
		else
		{
			//TODO: 불필요한 캡쳐 : BasePlayerState
			BasePlayerState->OnTeamChanged.AddLambda([this](const EPlayerTeam& ArgTeam)
			{
				if (ArgTeam == EPlayerTeam::A || ArgTeam == EPlayerTeam::B)
				{
					SetClientTeam(ArgTeam);
					//TODO: 이렇게 하기보다는 PlayersByTeamMap.Contains(PlayerTeam)을 사용하는 게 좋습니다.
					for (const auto temp : PlayersByTeamMap[ClientTeam])
					{
						CharacterSelectWidget->RegisterPlayer(temp);
						//현재까지 등록된 플레이어 스테이트들을 위젯에 등록한다
					}
				}
			});
		}
	}
}

void AOccupationGameState::DestroyTriggerBox()
{
	UWorld* World = GetWorld();
	if (World)
	{
		TArray<ATriggerBox*> TriggerBoxes;

		for (TActorIterator<ATriggerBox> ActorIterator(World); ActorIterator; ++ActorIterator)
		{
			ATriggerBox* TriggerBox = *ActorIterator;
			if (TriggerBox)
			{
				TriggerBoxes.Add(TriggerBox);
			}
		}

		for (ATriggerBox* TriggerBox : TriggerBoxes)
		{
			if (TriggerBox)
			{
				TriggerBox->Destroy();
			}
		}
	}
}

void AOccupationGameState::ShowEndResultWidget()
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		const auto LakayaPlayerState = Cast<ALakayaBasePlayerState>(
			LocalController->GetPlayerState<ALakayaBasePlayerState>());
		if (LakayaPlayerState == nullptr) UE_LOG(LogTemp, Warning, TEXT("LakayaPlayerState is null."));

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

		GameResultWidget->AntiScore->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), ATeamScore)));
		GameResultWidget->ProScore->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), BTeamScore)));

		ShowGradeResultWidget(LakayaPlayerState, LocalController);
	}
}

void AOccupationGameState::ShowGradeResultWidget(ALakayaBasePlayerState* PlayerState, APlayerController* Controller)
{
	// 게임 결과를 띄우고 5초뒤에 비활성화 해주고, GradeResultWidget, GradeResultElementWidget을 띄웁니다.
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, PlayerState, Controller]
	{
		GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
		GradeResultWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (PlayerState->GetTeam() == EPlayerTeam::A)
			GradeResultWidget->AntiTextBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (PlayerState->GetTeam() == EPlayerTeam::B)
			GradeResultWidget->ProTextBoxImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (PlayerState->IsSameTeam(GetOccupationWinner()))
			GradeResultWidget->VictoryImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		else GradeResultWidget->DefeatImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		Controller->SetShowMouseCursor(true);

		TapBool = true;
		ShowGradeResultElementWidget(PlayerState);
		BindDetailResultWidget();
	});
	GetWorldTimerManager().SetTimer(TimerHandle_GameResultHandle, TimerDelegate, 5.0f, false);
}

void AOccupationGameState::ShowGradeResultElementWidget(ALakayaBasePlayerState* NewPlayerState) const
{
	GradeResultElementWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (NewPlayerState->GetTeam() == EPlayerTeam::A)
	{
		ShowAntiTeamGradeResultElementWidget();
	}

	if (NewPlayerState->GetTeam() == EPlayerTeam::B)
	{
		ShowProTeamGradeResultElementWidget();
	}
}

void AOccupationGameState::ShowAntiTeamGradeResultElementWidget() const
{
	int index = 0;
	TArray<TObjectPtr<ALakayaBasePlayerState>> AntiPlayerArray;

	FString F_FormattedName;
	FString S_FormattedName;
	FString T_FormattedName;

	// 등록된 인원 만큼 정보 가져오기
	for (auto RegisterPlayerState : PlayersByTeamMap[EPlayerTeam::A])
	{
		// Anti팀 index번째의 정보를 AntiPlayerArray에 넣어줍니다.
		AntiPlayerArray.Add(RegisterPlayerState);

		switch (index)
		{
		// Anti팀 1등의 정보를 바인딩합니다.
		case 0:
			if (AntiPlayerArray[index]->GetCharacterName() == "Rena")
				GradeResultElementWidget->F_AntiRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (AntiPlayerArray[index]->GetCharacterName() == "Wazi")
				GradeResultElementWidget->F_AntiWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->F_AntiRankBoardImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			F_FormattedName = FString::Printf(TEXT("%s"), *AntiPlayerArray[index]->GetName());
			GradeResultElementWidget->F_NameText->SetText(FText::FromString(F_FormattedName));
			GradeResultElementWidget->F_NameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->F_TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				AntiPlayerArray[index]->GetTotalScore())));
			GradeResultElementWidget->F_TotalScoreText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->F_RankText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GradeResultElementWidget->F_ScoreBoardIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		// Anti팀 2등의 정보를 바인딩합니다.
		case 1:
			if (AntiPlayerArray[index]->GetCharacterName() == "Rena")
				GradeResultElementWidget->S_AntiRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (AntiPlayerArray[index]->GetCharacterName() == "Wazi")
				GradeResultElementWidget->S_AntiWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->S_AntiRankBoardImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			S_FormattedName = FString::Printf(TEXT("%s"), *AntiPlayerArray[index]->GetName());
			GradeResultElementWidget->S_NameText->SetText(FText::FromString(S_FormattedName));
			GradeResultElementWidget->S_NameText->SetVisibility(ESlateVisibility::HitTestInvisible);

			GradeResultElementWidget->S_TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				AntiPlayerArray[index]->GetTotalScore())));
			GradeResultElementWidget->S_TotalScoreText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->S_RankText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GradeResultElementWidget->S_ScoreBoardIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		// Anti팀 3등의 정보를 바인딩합니다.
		case 2:
			if (AntiPlayerArray[index]->GetCharacterName() == "Rena")
				GradeResultElementWidget->T_AntiRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (AntiPlayerArray[index]->GetCharacterName() == "Wazi")
				GradeResultElementWidget->T_AntiWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->T_AntiRankBoardImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			T_FormattedName = FString::Printf(TEXT("%s"), *AntiPlayerArray[index]->GetName());
			GradeResultElementWidget->T_NameText->SetText(FText::FromString(T_FormattedName));
			GradeResultElementWidget->T_NameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->T_TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				AntiPlayerArray[index]->GetTotalScore())));
			GradeResultElementWidget->T_TotalScoreText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->T_RankText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GradeResultElementWidget->T_ScoreBoardIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("존재하지 않은 Index 입니다."));
			break;
		}
		++index;
	}
}

void AOccupationGameState::ShowProTeamGradeResultElementWidget() const
{
	int index = 0;
	TArray<TObjectPtr<ALakayaBasePlayerState>> ProPlayerArray;

	FString F_FormattedName;
	FString S_FormattedName;
	FString T_FormattedName;

	// 등록된 인원 만큼 정보 가져오기
	for (auto RegisterPlayerState : PlayersByTeamMap[EPlayerTeam::B])
	{
		// Anti팀 index번째의 정보를 AntiPlayerArray에 넣어줍니다.
		ProPlayerArray.Add(RegisterPlayerState);

		switch (index)
		{
		// Anti팀 1등의 정보를 바인딩합니다.
		case 0:
			if (ProPlayerArray[index]->GetCharacterName() == "Rena")
				GradeResultElementWidget->F_ProRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (ProPlayerArray[index]->GetCharacterName() == "Wazi")
				GradeResultElementWidget->F_ProWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->F_ProRankBoardImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			F_FormattedName = FString::Printf(TEXT("%s"), *ProPlayerArray[index]->GetName());
			GradeResultElementWidget->F_NameText->SetText(FText::FromString(F_FormattedName));
			GradeResultElementWidget->F_NameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->F_TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				ProPlayerArray[index]->GetTotalScore())));
			GradeResultElementWidget->F_TotalScoreText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->F_RankText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GradeResultElementWidget->F_ScoreBoardIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		// Anti팀 2등의 정보를 바인딩합니다.
		case 1:
			if (ProPlayerArray[index]->GetCharacterName() == "Rena")
				GradeResultElementWidget->S_ProRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (ProPlayerArray[index]->GetCharacterName() == "Wazi")
				GradeResultElementWidget->S_ProWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->S_ProRankBoardImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			S_FormattedName = FString::Printf(TEXT("%s"), *ProPlayerArray[index]->GetName());
			GradeResultElementWidget->S_NameText->SetText(FText::FromString(S_FormattedName));
			GradeResultElementWidget->S_NameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->S_TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				ProPlayerArray[index]->GetTotalScore())));
			GradeResultElementWidget->S_TotalScoreText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->S_RankText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GradeResultElementWidget->S_ScoreBoardIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		// Anti팀 3등의 정보를 바인딩합니다.
		case 2:
			if (ProPlayerArray[index]->GetCharacterName() == "Rena")
				GradeResultElementWidget->T_ProRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (ProPlayerArray[index]->GetCharacterName() == "Wazi")
				GradeResultElementWidget->T_ProWaziImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->T_ProRankBoardImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			T_FormattedName = FString::Printf(TEXT("%s"), *ProPlayerArray[index]->GetName());
			GradeResultElementWidget->T_NameText->SetText(FText::FromString(T_FormattedName));
			GradeResultElementWidget->T_NameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->T_TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"),
				ProPlayerArray[index]->GetTotalScore())));
			GradeResultElementWidget->T_TotalScoreText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			GradeResultElementWidget->T_RankText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			GradeResultElementWidget->T_ScoreBoardIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("존재하지 않은 Index 입니다."));
			break;
		}

		++index;
	}
}

void AOccupationGameState::BindDetailResultWidget()
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		const ALakayaBasePlayerState* PlayerState = Cast<ALakayaBasePlayerState>(LocalController->PlayerState);

		// Anti
		if (PlayerState->GetTeam() == EPlayerTeam::A)
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
				DetailResultWidget->PortraitRenaImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				DetailResultWidget->UserBoxCharacterNameText->SetText(FText::FromString(TEXT("WAZI")));
			}
		}

		// Pro
		if (PlayerState->GetTeam() == EPlayerTeam::B)
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
		FormattedName = FString::Printf(TEXT("%s"), *PlayerState->GetName());
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

	for (auto& Element : PlayersByTeamMap[EPlayerTeam::A])
	{
		DetailResultWidget->RegisterPlayer(Element);
	}

	for (auto& Element : PlayersByTeamMap[EPlayerTeam::B])
	{
		DetailResultWidget->RegisterPlayer(Element);
	}
}
