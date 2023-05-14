#include "GameMode/OccupationGameState.h"
#include "UI/TeamScoreWidget.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

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
}

void AOccupationGameState::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		if (TeamScoreWidgetClass)
		{
			TeamScoreWidget = CreateWidget<UTeamScoreWidget>(LocalController, TeamScoreWidgetClass);
			if (TeamScoreWidget == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_TeamScoreWidget is null."));
				return;
			}
			TeamScoreWidget->AddToViewport();
			TeamScoreWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_LocalPlayerController is null."));
		return;
	}
}

void AOccupationGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (IsValid(TeamScoreWidget))
		TeamScoreWidget->SetVisibility(ESlateVisibility::Visible);
}

void AOccupationGameState::NotifyKillCharacter_Implementation(AController* KilledController, AActor* KilledActor,
                                                              AController* EventInstigator, AActor* Causer)
{
	OnKillCharacterNotify.Broadcast(KilledController, KilledActor, EventInstigator, Causer);
}

void AOccupationGameState::SetOccupationWinner()
{
	CurrentOccupationWinner = ATeamScore > BTeamScore ? EPlayerTeam::A : EPlayerTeam::B;
	OnChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
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

	UE_LOG(LogTemp, Warning, TEXT("ATeamScore : %f"), ATeamScore);
	UE_LOG(LogTemp, Warning, TEXT("BTeamScore : %f"), BTeamScore);
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