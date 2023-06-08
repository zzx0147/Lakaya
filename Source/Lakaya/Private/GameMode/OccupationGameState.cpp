#include "GameMode/OccupationGameState.h"

#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Engine/TriggerBox.h"
#include "ETC/OutlineManager.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "GameMode/OccupationGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Occupation/ShieldWallObject.h"
#include "UI/DetailResultWidget.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GameResultWidget.h"
#include "UI/GradeResultElementWidget.h"
#include "UI/GradeResultWidget.h"
#include "UI/MatchStartWaitWidget.h"
#include "UI/StartMessageWidget.h"
#include "UI/TeamScoreWidget.h"
#include "UI/DetailResultElementWidget.h"

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

void AOccupationGameState::OnLocalPlayerControllerPlayerStateUpdated(APlayerController* LocalPlayerController)
{
	Super::OnLocalPlayerControllerPlayerStateUpdated(LocalPlayerController);
	if (!LocalPlayerController) return;
	if (const auto PlayerState = LocalPlayerController->GetPlayerState<ALakayaBasePlayerState>())
	{
		SetClientTeam(PlayerState->GetTeam());
		PlayerState->OnTeamChanged.AddUObject(this, &AOccupationGameState::SetClientTeam);
	}
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

		if (DetailResultElementWidgetClass)
		{
			DetailResultElementWidget = CreateWidget<UDetailResultElementWidget>(LocalController, DetailResultElementWidgetClass);
			if (DetailResultElementWidget.IsValid())
			{
				DetailResultElementWidget->AddToViewport(1);
				DetailResultElementWidget->SetVisibility(ESlateVisibility::Hidden);
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
		DestroyShieldWallObject();
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
	BindDetailResultWidget();
	BindDetailResultElementWidget();
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
	DetailResultElementWidget->SetVisibility(detailVisibility);

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
		RegisterPlayerByTeam(BasePlayerState->GetTeam(), BasePlayerState);
		BasePlayerState->OnTeamChanged.AddUObject(this, &AOccupationGameState::RegisterPlayerByTeam, BasePlayerState);
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

	//현재까지 등록된 플레이어 스테이트들을 위젯에 등록한다
	if (!PlayersByTeamMap.Contains(ClientTeam)) return;
	for (const auto& Temp : PlayersByTeamMap[ClientTeam]) CharacterSelectWidget->RegisterPlayer(Temp);
}

void AOccupationGameState::DestroyShieldWallObject()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AShieldWallObject::StaticClass(), FoundActors);

	for (AActor* ShieldActor : FoundActors)
		ShieldActor->Destroy();
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

void AOccupationGameState::GradeResultTeamInfo(TArray<TObjectPtr<ALakayaBasePlayerState>>& NewPlayerArray,
                                               uint8 NewIndex) const
{
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
	if (NewPlayerArray[NewIndex]->GetTeam() == EPlayerTeam::A)
		TeamLetter = "Anti";
	if (NewPlayerArray[NewIndex]->GetTeam() == EPlayerTeam::B)
		TeamLetter = "Pro";

	GradeResultElementWidget->GetWidgetFromName(
		*FString::Printf(TEXT("%s_%s_%s_Image"), *RankLetter, *TeamLetter, *CharacterName))->SetVisibility(
		ESlateVisibility::SelfHitTestInvisible);
	GradeResultElementWidget->GetWidgetFromName(
		*FString::Printf(TEXT("%s_%s_RankBoard_Image"), *RankLetter, *TeamLetter))->SetVisibility(
		ESlateVisibility::SelfHitTestInvisible);

	FString FormattedName = FString::Printf(TEXT("%s"), *PlayerArray[NewIndex]->GetName());
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
	for (auto RegisterPlayerState : PlayersByTeamMap[EPlayerTeam::A])
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
	for (auto RegisterPlayerState : PlayersByTeamMap[EPlayerTeam::B])
	{
		ProPlayerArray.Add(RegisterPlayerState);
		GradeResultTeamInfo(ProPlayerArray, index);
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
}

void AOccupationGameState::BindDetailResultElementWidget()
{
	uint8 AntiIndex = 0;
	uint8 ProIndex = 0;

	for (auto& ElementPlayerState : PlayersByTeamMap[EPlayerTeam::A])
	{
		DetailResultElementWidget->SetElementWidget(ElementPlayerState, AntiIndex);
		++AntiIndex;
	}

	for (auto& ElementPlayerState : PlayersByTeamMap[EPlayerTeam::B])
	{
		DetailResultElementWidget->SetElementWidget(ElementPlayerState, ProIndex);
		++ProIndex;
	}
}

void AOccupationGameState::RegisterPlayerByTeam(const EPlayerTeam& Team, ALakayaBasePlayerState* PlayerState)
{
	if (!PlayersByTeamMap.Contains(Team)) return;
	auto& PlayerStates = PlayersByTeamMap[Team];
	PlayerStates.Emplace(PlayerState);
	PlayerState->SetUniqueStencilMask(GetUniqueStencilMaskByTeamAndIndex(Team, PlayerStates.Num()));
	PlayerState->SetAlly(ClientTeam == Team);
	if (ClientTeam == Team && CharacterSelectWidget) CharacterSelectWidget->RegisterPlayer(PlayerState);
}

ERendererStencilMask AOccupationGameState::GetUniqueStencilMaskByTeamAndIndex(const EPlayerTeam& Team,
                                                                              const uint8& Count) const
{
	switch (Count)
	{
	case 1: return ClientTeam == Team ? ERendererStencilMask::ERSM_1 : ERendererStencilMask::ERSM_8;
	case 2: return ClientTeam == Team ? ERendererStencilMask::ERSM_2 : ERendererStencilMask::ERSM_16;
	case 3: return ClientTeam == Team ? ERendererStencilMask::ERSM_4 : ERendererStencilMask::ERSM_32;
	default: return ERendererStencilMask::ERSM_Default;
	}
}
