// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/OccupationGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Character/ArmedCharacter.h"
#include "Character/BattlePlayerController.h"
#include "Character/CollectorPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameMode/OccupationGameState.h"

AOccupationGameMode::AOccupationGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/Characters/LakayaCharacter/Dummy/BP_PlayerDummy"));
	if (!PlayerPawnObject.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("OccupationGameMode_Failed to find player pawn blueprint."));
		return;
	}

	DefaultPawnClass = PlayerPawnObject.Class;
	PlayerControllerClass = ABattlePlayerController::StaticClass();
	PlayerStateClass = ACollectorPlayerState::StaticClass();
	GameStateClass = AOccupationGameState::StaticClass();
}

void AOccupationGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AOccupationGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnGameModeInitialized.Broadcast();
}

void AOccupationGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("The Player has entered the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), GetNumPlayers());

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("플레이어가 입장했습니다."));

	AOccupationGameState* OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_OccupationGameState is null."));
		return;
	}

	int32 CurrentPlayerNum = OccupationGameState->PlayerArray.Num();
	OccupationGameState->SetNumPlayers(CurrentPlayerNum);

	if (GetNumPlayers() >= OccupationGameState->GetMaxPlayers())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedStart, this, &AOccupationGameMode::DelayedStartMatch, 5.0f, false);
	}
}

void AOccupationGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode_OccupationGameState is null."));
		return;
	}
	
	OccupationGameState->SetGameState(EOccupationGameState::StandByToPregressLoading);

	
	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchIsWaitingToStart"));

	// 게임시작 조건
	ReadyToStartMatch();
}

bool AOccupationGameMode::ReadyToStartMatch_Implementation()
{
	if (GetMatchState() != MatchState::WaitingToStart)
	{
		return false;
	}

	if (!bWaitToStart) return false;

	AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_OccupationGameState is null."));
		return false;
	}
	
	OccupationGameState->SetGameState(EOccupationGameState::Progress);
	
	return true;
}

void AOccupationGameMode::DelayedStartMatch()
{
	bWaitToStart = true;
}

void AOccupationGameMode::HandleMatchHasStarted()
{
	// 게임 시작 후, 서버 측 클라에게 UI 바인딩.
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

void AOccupationGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasEnded"));
}

void AOccupationGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleLeavingMap"));
}

void AOccupationGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ADamageableCharacter* DamageableCharacter = Cast<ADamageableCharacter>(Exiting->GetPawn());
	if (DamageableCharacter)
	{
		DamageableCharacter->OnKillCharacterNotify.RemoveAll(this);	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_DamageableCharacter is null."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("The Player has left the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);
}

void AOccupationGameMode::OnKilledCharacter(AController* VictimController, AActor* Victim,
	AController* InstigatorController, AActor* DamageCauser)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADamageableCharacter::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		ADamageableCharacter* MyActor = Cast<ADamageableCharacter>(Actor);
		if (MyActor)
		{
			MyActor->OnKillCharacterNotify.AddUObject(this, &AOccupationGameMode::OnKilledCharacter);
			UE_LOG(LogTemp, Warning, TEXT("PlayerController OnKillCharacterNotify Binding."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MyActor is null."));
			return;
		}
	}
}

void AOccupationGameMode::OnKillNotifyBinding()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADamageableCharacter::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		ADamageableCharacter* MyActor = Cast<ADamageableCharacter>(Actor);
		if (MyActor)
		{
			MyActor->OnKillCharacterNotify.AddUObject(this, &AOccupationGameMode::OnKilledCharacter);
			UE_LOG(LogTemp, Warning, TEXT("PlayerController OnKillCharacterNotify Binding."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MyActor is null."));
			return;
		}
	}
}

void AOccupationGameMode::RespawnPlayer(AController* KilledController)
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

	ACollectorPlayerState* KilledPlayerState = Cast<ACollectorPlayerState>(KilledController->GetCharacter()->GetController()->PlayerState);
	if (KilledController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledPlayerState is null."));
		return;
	}

	ADamageableCharacter* KilledDamageableCharacter = Cast<ADamageableCharacter>(KilledCharacterActor);
	if (KilledDamageableCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledDamageableCharacter is null."));
		return;
	}
	
	// KilledPlayerState->ResetEnergy();
	KilledDamageableCharacter->FullHealth();
	KilledDamageableCharacter->Respawn();
}