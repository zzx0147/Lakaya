// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/LakayaDefalutPlayGameMode.h"

#include "Character/ArmedCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ALakayaDefalutPlayGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALakayaDefalutPlayGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALakayaDefalutPlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	UE_LOG(LogTemp, Warning, TEXT("The Player has entered the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), GetNumPlayers());

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("플레이어가 입장했습니다."));

}

void ALakayaDefalutPlayGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	
}

bool ALakayaDefalutPlayGameMode::ReadyToStartMatch_Implementation()
{
	return Super::ReadyToStartMatch_Implementation();
}

void ALakayaDefalutPlayGameMode::DelayedStartMatch()
{
	bWaitToStart = true;
}

void ALakayaDefalutPlayGameMode::HandleMatchHasStarted()
{
	// 게임 시작 후, 서버 측 클라에게 UI바인딩.
	Super::HandleMatchHasStarted();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APawn* PlayerPawn = PlayerController->GetPawn();
	AArmedCharacter* Armed = Cast<AArmedCharacter>(PlayerPawn);
	if (Armed)
	{
		Armed->CallBeginPlay();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast pawn to AArmedCharacter"));
	}

	OnKillNotifyBinding();
	
	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void ALakayaDefalutPlayGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasEnded"));
}

void ALakayaDefalutPlayGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleLeavingMap"));
}

void ALakayaDefalutPlayGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	ADamageableCharacter* DamageableCharacter = Cast<ADamageableCharacter>(Exiting->GetPawn());
	if (DamageableCharacter)
	{
		DamageableCharacter->OnKillCharacterNotify.RemoveAll(this);	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode_DamageableCharacter is null."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("The Player has left the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);
}

void ALakayaDefalutPlayGameMode::OnKilledCharacter(AController* VictimController, AActor* Victim,
	AController* InstigatorController, AActor* DamageCauser)
{
	FTimerHandle* ExistingTimer = RespawnTimers.Find(VictimController);
	if (ExistingTimer != nullptr)
	{
		GetWorldTimerManager().ClearTimer(*ExistingTimer);
		RespawnTimers.Remove(VictimController);
	}
	
	TArray<AController*> DeadPlayers;
	DeadPlayers.Add(VictimController);
	
	for (auto& Pair : RespawnTimers)
	{
		FTimerHandle& Timer = Pair.Value;
		AController* Player = Pair.Key;
	
		if (GetWorldTimerManager().IsTimerActive(Timer))
		{
			DeadPlayers.Add(Player);
		}
	}
	
	for (AController* DeadPlayer : DeadPlayers)
	{
		FTimerHandle NewTimer;
		GetWorldTimerManager().SetTimer(NewTimer, [this, DeadPlayer](){ RespawnPlayer(DeadPlayer); }, PlayerRespawnTime, false);
		RespawnTimers.Add(DeadPlayer, NewTimer);
	}
}

void ALakayaDefalutPlayGameMode::OnKillNotifyBinding()
{
}

void ALakayaDefalutPlayGameMode::RespawnPlayer(AController* KilledController)
{
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

	ADamageableCharacter* KilledDamageableCharacter = Cast<ADamageableCharacter>(KilledCharacterActor);
	if (KilledDamageableCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledDamageableCharacter is null."));
		return;
	}
	
	KilledDamageableCharacter->FullHealth();
	KilledDamageableCharacter->Respawn();
}
