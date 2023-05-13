// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/LakayaDefaultPlayGameMode.h"

#include "Character/ArmedCharacter.h"
#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/LakayaBaseGameState.h"

namespace MatchState
{
	 const FName IsSelectCharacter = FName(TEXT("IsSelectCharacter"));
}

ALakayaDefaultPlayGameMode::ALakayaDefaultPlayGameMode()
{
	PlayerRespawnTime = 3;
	bWaitToStart = false;
	MinRespawnDelay = 5.0f;
	//CharacterSelectTime = 10.0f;
}

void ALakayaDefaultPlayGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALakayaDefaultPlayGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALakayaDefaultPlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	UE_LOG(LogTemp, Warning, TEXT("The Player has entered the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), GetNumPlayers());

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("플레이어가 입장했습니다."));

	if (const auto BasePlayerState = NewPlayer->GetPlayerState<ALakayaBasePlayerState>())
	{
		BasePlayerState->OnCharacterNameChanged.AddLambda(
			[this, NewPlayer](ALakayaBasePlayerState* ArgBasePlayerState, const FName& ArgCharacterName){

				if (GetGameState<ALakayaBaseGameState>()->GetMatchState() == MatchState::InProgress)
				{
					if (auto PlayerPawn = NewPlayer->GetPawn())
					{
						NewPlayer->UnPossess();
						PlayerPawn->Destroy();
						RestartPlayer(NewPlayer);
					}
				}
			});

		BasePlayerState->OnPlayerKilled.AddUObject(this, &ALakayaDefaultPlayGameMode::OnPlayerKilled);
	}


}

void ALakayaDefaultPlayGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	if (MatchState == MatchState::IsSelectCharacter)
	{
		HandleMatchIsSelectCharacter();
	}
}


void ALakayaDefaultPlayGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

bool ALakayaDefaultPlayGameMode::ReadyToStartMatch_Implementation()
{
	return Super::ReadyToStartMatch_Implementation();
}

void ALakayaDefaultPlayGameMode::HandleMatchIsSelectCharacter()
{
	FTimerHandle TimerHandler;
	//GetWorldTimerManager().SetTimer(TimerHandler, this, &ALakayaDefaultPlayGameMode::StartMatch, 10.0f, false);
}

void ALakayaDefaultPlayGameMode::HandleMatchHasStarted()
{
	// 게임 시작 후, 서버 측 클라에게 UI바인딩.
	Super::HandleMatchHasStarted();
	
	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void ALakayaDefaultPlayGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasEnded"));
}

void ALakayaDefaultPlayGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleLeavingMap"));
}

void ALakayaDefaultPlayGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UE_LOG(LogTemp, Warning, TEXT("The Player has left the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);
}

void ALakayaDefaultPlayGameMode::OnPlayerKilled(AController* VictimController, AController* InstigatorController, AActor* DamageCauser)
{
	if (const auto InstigatorPlayerState = InstigatorController->GetPlayerState<ALakayaBasePlayerState>())
		InstigatorPlayerState->IncreaseKillCount();

	const auto VictimPlayerState = VictimController->GetPlayerState<ALakayaBasePlayerState>();
	if (VictimPlayerState != nullptr) VictimPlayerState->IncreaseDeathCount();

	if (const auto BaseGameState = GetGameState<ALakayaBaseGameState>())
		BaseGameState->NotifyPlayerKilled(VictimController, InstigatorController, DamageCauser);

	if (ShouldRespawn())
	{
		VictimPlayerState->SetRespawnTimer(GetGameState<AGameState>()->GetServerWorldTimeSeconds() + MinRespawnDelay, this, &ALakayaDefaultPlayGameMode::RespawnPlayer);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("RespawnTimerSetted!!!"));
	}
	else
	{
		VictimPlayerState->SetRespawnTimer(-1.0f, this);
	}


	//FTimerHandle* ExistingTimer = RespawnTimers.Find(VictimController);
	//if (ExistingTimer != nullptr)
	//{
	//	GetWorldTimerManager().ClearTimer(*ExistingTimer);
	//	RespawnTimers.Remove(VictimController);
	//}

	//RespawnTimers.Add(VictimController, FTimerHandle());
	//FTimerHandle& NewTimer = RespawnTimers[VictimController];
	//GetWorldTimerManager().SetTimer(NewTimer, [this, VictimController]() { RespawnPlayer(VictimController); }, PlayerRespawnTime, false);
} 

void ALakayaDefaultPlayGameMode::StartSelectCharacter()
{
	if (MatchState != MatchState::WaitingToStart) return;

	SetMatchState(MatchState::IsSelectCharacter);
}

bool ALakayaDefaultPlayGameMode::HasMatchStarted() const
{
	if (MatchState == MatchState::IsSelectCharacter) return false;

	return Super::HasMatchStarted();
}

UClass* ALakayaDefaultPlayGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const auto PlayerState = InController->GetPlayerState<ALakayaBasePlayerState>())
		if (CharacterClasses.Contains(PlayerState->GetCharacterName()))
			return CharacterClasses[PlayerState->GetCharacterName()];

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ALakayaDefaultPlayGameMode::RespawnPlayer(AController* KilledController)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("RespawnPlayer!!!!"));


	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);

	if (PlayerStartActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No player start actors found."));
		return;
	}

	APlayerStart* RandomPlayerStart = Cast<APlayerStart>(PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() - 1)]);

	APawn* KilledPawn = Cast<APawn>(KilledController->GetPawn());
	ACharacter* KilledCharacterActor = Cast<ACharacter>(KilledController->GetCharacter());
	
	if (KilledPawn != nullptr)
	{
		KilledPawn->SetActorLocation(RandomPlayerStart->GetActorLocation());
	}
	else if (KilledController != nullptr)
	{
		KilledCharacterActor->SetActorLocation(RandomPlayerStart->GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledCharacter is not a pawn or an actor."));
		return;
	}
	
	// ADamageableCharacter* KilledDamageableCharacter = Cast<ADamageableCharacter>(KilledCharacterActor);
	// if (KilledDamageableCharacter == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("KilledDamageableCharacter is null."));
	// 	return;
	// }
	//
	// KilledDamageableCharacter->Respawn();
}

bool ALakayaDefaultPlayGameMode::ShouldRespawn()
{
	return true;
}
