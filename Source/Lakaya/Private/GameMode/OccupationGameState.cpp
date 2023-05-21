#include "GameMode/OccupationGameState.h"
#include "Blueprint/UserWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "GameMode/OccupationGameMode.h"
#include "Net/UnrealNetwork.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/GameResultWidget.h"
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
	MatchDuration = 10.f;
	ClientTeam = EPlayerTeam::None;

	PlayersByTeamMap.Add(EPlayerTeam::A, TArray<ALakayaBasePlayerState*>());
	PlayersByTeamMap.Add(EPlayerTeam::B, TArray<ALakayaBasePlayerState*>());
	//TODO: Emplace는 항상 Add와 같은 속도를 보여주거나 더 빠릅니다. 또 키값만 지정해도 값형식의 밸류는 알아서 기본생성됩니다.
	// PlayersByTeamMap.Emplace(EPlayerTeam::A);
	// PlayersByTeamMap.Emplace(EPlayerTeam::B);
}

void AOccupationGameState::BeginPlay()
{
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		if (TeamScoreWidgetClass)
		{
			TeamScoreWidget = CreateWidget<UTeamScoreWidget>(LocalController, TeamScoreWidgetClass);
			if (TeamScoreWidget == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_TeamScoreWidget is null."));
				//TODO: 리턴을 하면 다른 멀쩡한 위젯들이 생성되지 않고 넘어갈 수 있습니다.
				return;
			}
			TeamScoreWidget->AddToViewport();
			TeamScoreWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if (GameResultWidgetClass)
		{
			GameResultWidget = CreateWidget<UGameResultWidget>(LocalController, GameResultWidgetClass);
			if (GameResultWidget.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("시발 있어요 위젯.시발 있어요 위젯.시발 있어요 위젯.시발 있어요 위젯.시발 있어요 위젯."));
				GameResultWidget->AddToViewport();
				GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_LocalPlayerController is null."));
		return;
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
}

void AOccupationGameState::NotifyKillCharacter_Implementation(AController* KilledController, AActor* KilledActor,
                                                              AController* EventInstigator, AActor* Causer)
{
	//OnKillCharacterNotify.Broadcast(KilledController, KilledActor, EventInstigator, Causer);
}

void AOccupationGameState::EndTimeCheck()
{
	if (((ATeamScore >= MaxScore || BTeamScore >= MaxScore)))
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
		EPlayerTeam PlayerTeam = BasePlayerState->GetTeam();
		//TODO: 이렇게 하기보다는 PlayersByTeamMap.Contains(PlayerTeam)을 사용하는 게 좋습니다.
		if (PlayerTeam == EPlayerTeam::A || PlayerTeam == EPlayerTeam::B)//���� ������ �ٷ� �迭�� �߰�
		{
			PlayersByTeamMap[PlayerTeam].Add(BasePlayerState);
			//TODO: Emplace는 항상 Add보다 낫습니다.
			//PlayersByTeamMap[PlayerTeam].Emplace(BasePlayerState);
			
			if (ClientTeam == PlayerTeam)
			{
				CharacterSelectWidget->RegisterPlayer(BasePlayerState);
			}
		}
		//TODO: 팀이 없는 경우에만 람다를 등록하기보다는, 팀이 있더라도 게임도중 팀이 변경되는 경우를 상정하여 그냥 람다를 등록하는 것이 좋을 것 같습니다.
		else//������ ���� ����Ǿ����� �߰�
		{
			BasePlayerState->OnTeamChanged.AddLambda([this, BasePlayerState](const EPlayerTeam& ArgTeam)
				{
					if (ArgTeam == EPlayerTeam::A)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("TeamChange! A!"));
					}
					else if (ArgTeam == EPlayerTeam::B)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("TeamChange! B!"));
					}
					else if (ArgTeam == EPlayerTeam::None)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("TeamChange! NONE!"));
					}
					else if (ArgTeam == EPlayerTeam::Individual)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("TeamChange! Individual!"));
					}

					//TODO: 이렇게 하기보다는 PlayersByTeamMap.Contains(PlayerTeam)을 사용하는 게 좋습니다.
					if (ArgTeam == EPlayerTeam::A || ArgTeam == EPlayerTeam::B)
					{
						//TODO: Emplace는 항상 Add보다 낫습니다.
						PlayersByTeamMap[ArgTeam].Add(BasePlayerState);
						if (ClientTeam == ArgTeam)//Ŭ���̾�Ʈ ���� NONE �� �� ����
						{
							CharacterSelectWidget->RegisterPlayer(BasePlayerState);
						}
					}
				});
		}
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

void AOccupationGameState::CreateCharacterSelectWidget(APlayerController* LocalController)
{
	Super::CreateCharacterSelectWidget(LocalController);

	if (const auto BasePlayerState = LocalController->GetPlayerState<ALakayaBasePlayerState>())
	{
		const auto NewTeam = BasePlayerState->GetTeam();
		if (NewTeam == EPlayerTeam::A || NewTeam == EPlayerTeam::B)
		{
			ClientTeam = NewTeam;//로컬 컨트롤러의 팀을 저장한다.(로컬 컨트롤러의 팀에 따라 표기할 팀이 달라짐)
			
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
			BasePlayerState->OnTeamChanged.AddLambda([this, BasePlayerState](const EPlayerTeam& ArgTeam)
				{
					if (ArgTeam == EPlayerTeam::A || ArgTeam == EPlayerTeam::B)
					{
						ClientTeam = ArgTeam;
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
