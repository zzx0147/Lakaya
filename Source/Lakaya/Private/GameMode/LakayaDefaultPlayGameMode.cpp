// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/LakayaDefaultPlayGameMode.h"

#include "EngineUtils.h"
#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/CapsuleComponent.h"
#include "EOS/EOSGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "GameMode/LakayaBaseGameState.h"
#include "Kismet/GameplayStatics.h"

// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
// namespace MatchState
// {
// 	 const FName IsSelectCharacter = FName(TEXT("IsSelectCharacter"));
// }

const FString ALakayaDefaultPlayGameMode::ATeamSpawnTag = FString(TEXT("ATeamSpawnZone"));
const FString ALakayaDefaultPlayGameMode::BTeamSpawnTag = FString(TEXT("BTeamSpawnZone"));

ALakayaDefaultPlayGameMode::ALakayaDefaultPlayGameMode()
{
	bDelayedStart = true;
	MinRespawnDelay = 4.0f;
	// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
	// CharacterSelectStartDelay = 3.0f;
	AgonesSDK = CreateDefaultSubobject<UAgonesComponent>(TEXT("AgonesSDK"));
	AgonesSDK->bDisableAutoConnect = false;
}

void ALakayaDefaultPlayGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	FString SpawnTag;
	if (const auto BasePlayerState = NewPlayer->GetPlayerState<ALakayaBasePlayerState>())
	{
		switch (BasePlayerState->GetTeam())
		{
		case ETeam::Anti:
			SpawnTag = ATeamSpawnTag;
			break;
		case ETeam::Pro:
			SpawnTag = BTeamSpawnTag;
			break;
		default:
			SpawnTag = TEXT("");
			break;
		}
	}

	//TODO: RestartPlayer를 오버라이딩할 필요 없이, FindPlayerStart에서 더 간단히 구현할 수 있습니다.
	AActor* StartSpot = FindPlayerStart(NewPlayer, SpawnTag);

	// If a start spot wasn't found,
	if (StartSpot == nullptr)
	{
		// Check for a previously assigned spot
		if (NewPlayer->StartSpot != nullptr)
		{
			StartSpot = NewPlayer->StartSpot.Get();
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Player start not found, using last start spot"));
		}
	}

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}

void ALakayaDefaultPlayGameMode::InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer)
{
	if (StartSpot == nullptr) return;

	if (const auto Pawn = NewPlayer->GetPawn())
	{
		Pawn->SetActorLocation(StartSpot->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);
	}
}

AActor* ALakayaDefaultPlayGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	//TODO: 이렇게 하기보다는 플레이어의 팀에 따라서 미리 저장해둔 플레이어 스타트 중 겹치지 않는 플레이어 스타트를 찾아서 리턴하도록 하고,
	// 팀이 없거나 하는 경우에는 간단히 Super::FindPlayerStart_Implementation을 호출해주는 편이 나을 것 같습니다.
	UWorld* World = GetWorld();

	// If incoming start is specified, then just use it
	if (!IncomingName.IsEmpty())
	{
		const FName IncomingPlayerStartTag = FName(*IncomingName);
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* Start = *It;
			if (Start && Start->PlayerStartTag == IncomingPlayerStartTag)
			{
				if (const auto Capsule = Start->GetCapsuleComponent())
				{
					TSet<AActor*> OverlappingActors;
					Capsule->GetOverlappingActors(OverlappingActors,AInteractableCharacter::StaticClass());
					if (OverlappingActors.Num() == 0)
						return Start;
				}
			}
		}
	}

	// Always pick StartSpot at start of match
	if (ShouldSpawnAtStartSpot(Player))
	{
		if (AActor* PlayerStartSpot = Player->StartSpot.Get())
		{
			return PlayerStartSpot;
		}
		else
		{
			UE_LOG(LogGameMode, Error, TEXT("FindPlayerStart: ShouldSpawnAtStartSpot returned true but the Player StartSpot was null."));
		}
	}

	AActor* BestStart = ChoosePlayerStart(Player);
	if (BestStart == nullptr)
	{
		// No player start found
		UE_LOG(LogGameMode, Log, TEXT("FindPlayerStart: PATHS NOT DEFINED or NO PLAYERSTART with positive rating"));

		// This is a bit odd, but there was a complex chunk of code that in the end always resulted in this, so we may as well just 
		// short cut it down to this.  Basically we are saying spawn at 0,0,0 if we didn't find a proper player start
		BestStart = World->GetWorldSettings();
	}

	return BestStart;
}

void ALakayaDefaultPlayGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Occupation Game Mode Begin Play"));
}

//TODO: 사용되지 않는 오버라이딩 제거
void ALakayaDefaultPlayGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALakayaDefaultPlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Warning, TEXT("The Player has entered the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), GetNumPlayers());
	RegisterPlayer(NewPlayer);
}

void ALakayaDefaultPlayGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
	// if (MatchState == MatchState::IsSelectCharacter)
	// {
		// HandleMatchIsSelectCharacter();
	// }
}

// TODO : 캐릭터 선택 상태가 사라져서 캐릭터 선택 창에서 이루어 졌던 기능들을 옮겨야 합니다.
// TODO : 캐릭터가 생성이 되고 나서 캐릭터가 선택이 되기 때문에 여기서 진행하기 보다는 HandleMatchHasStarted에서 진행해야 합니다.
void ALakayaDefaultPlayGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

// TODO : 위의 내용과 동일합니다.
bool ALakayaDefaultPlayGameMode::ReadyToStartMatch_Implementation()
{
	return Super::ReadyToStartMatch_Implementation();
}

// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
// void ALakayaDefaultPlayGameMode::HandleMatchIsSelectCharacter()
// {
// 	FTimerHandle TimerHandler;
// 	//GetWorldTimerManager().SetTimer(TimerHandler, this, &ALakayaDefaultPlayGameMode::StartMatch, 10.0f, false);
// }

void ALakayaDefaultPlayGameMode::HandleMatchHasStarted()
{
	// 게임 시작 후, 서버 측 클라에게 UI바인딩.
	Super::HandleMatchHasStarted();
	
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void ALakayaDefaultPlayGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	if (const auto GameInstance = GetGameInstance<UEOSGameInstance>())
	{
		GameInstance->EndSession();
	}

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasEnded"));
}

//TODO: 사용되지 않는 오버라이딩 제거
void ALakayaDefaultPlayGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();

	UE_LOG(LogTemp, Error, TEXT("HandleLeavingMap"));
}

//TODO: 사용되지 않는 오버라이딩 제거
void ALakayaDefaultPlayGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UE_LOG(LogTemp, Warning, TEXT("The Player has left the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);
}

void ALakayaDefaultPlayGameMode::OnPlayerKilled(AController* VictimController, AController* InstigatorController, AActor* DamageCauser)
{
	if (const auto InstigatorPlayerState = InstigatorController->GetPlayerState<ALakayaBasePlayerState>())
	{
		InstigatorPlayerState->IncreaseKillCount();
		InstigatorPlayerState->AddTotalScoreCount(100);
	}

	const auto VictimPlayerState = VictimController->GetPlayerState<ALakayaBasePlayerState>();
	if (VictimPlayerState != nullptr) VictimPlayerState->IncreaseDeathCount();

	if (const auto LakayaBaseGameState = GetGameState<ALakayaBaseGameState>())
		LakayaBaseGameState->NotifyPlayerKilled(VictimController->GetPlayerState<APlayerState>(), InstigatorController->GetPlayerState<APlayerState>(), DamageCauser);

	//TODO: ShouldRespawn 함수는 사망한 플레이어가 부활할 수 있는지 여부를 검사하기 위해 기획되었습니다. 따라서 매개변수로 플레이어 스테이트나 컨트롤러를 받아야 합니다.
	if (ShouldRespawn())
	{
		static FRespawnTimerDelegate Delegate;
		Delegate.BindUObject(this, &ALakayaDefaultPlayGameMode::RespawnPlayer);
		VictimPlayerState->SetRespawnTimer(GetGameState<AGameState>()->GetServerWorldTimeSeconds() + MinRespawnDelay,
		                                   Delegate);
		// GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("RespawnTimerSetted!!!"));
	}
	else
	{
		VictimPlayerState->SetRespawnTimer(-1.0f);
	}
} 

// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
// void ALakayaDefaultPlayGameMode::StartSelectCharacter()
// {
// 	if (MatchState != MatchState::WaitingToStart) return;
//
// 	SetMatchState(MatchState::IsSelectCharacter);
// }

void ALakayaDefaultPlayGameMode::DelayedEndedGame()
{
	//TODO: UGameplayStatics::OpenLevelBySoftObjectPtr()를 사용하면 하드코딩을 줄일 수 있습니다.
	UGameplayStatics::OpenLevel(GetWorld(), "MainLobbyLevel");
}

bool ALakayaDefaultPlayGameMode::HasMatchStarted() const
{
	// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
	// return MatchState == MatchState::IsSelectCharacter ? false : Super::HasMatchStarted();
	// if (MatchState == MatchState::IsSelectCharacter) return false;

	return Super::HasMatchStarted();
}

//TODO: 사용되지 않는 함수
void ALakayaDefaultPlayGameMode::PlayerInitializeSetLocation(uint8 PlayersNum)
{
	// TODO
}

UClass* ALakayaDefaultPlayGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const auto PlayerState = InController->GetPlayerState<ALakayaBasePlayerState>())
		if (CharacterClasses.Contains(PlayerState->GetCharacterName()))
			return CharacterClasses[PlayerState->GetCharacterName()];

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

//TODO: 필요없는 함수 랩핑
void ALakayaDefaultPlayGameMode::RespawnPlayer(AController* KilledController)
{
	RestartPlayer(KilledController);
}

bool ALakayaDefaultPlayGameMode::ShouldRespawn()
{
	return true;
}

void ALakayaDefaultPlayGameMode::RegisterPlayer(AController* NewPlayer)
{
	if (const auto BasePlayerState = NewPlayer->GetPlayerState<ALakayaBasePlayerState>())
	{
		//TODO: NewPlayer를 캡쳐할 필요 없이 ArgBasePlayerState를 사용하면 됩니다.
		BasePlayerState->OnCharacterNameChanged.AddLambda(
			[this, NewPlayer](ALakayaBasePlayerState* ArgBasePlayerState, const FName& ArgCharacterName){

				//TODO: 매치스테이트는 게임모드에도 있습니다. IsMatchInProgress()를 사용하면 됩니다.
				if (GetGameState<ALakayaBaseGameState>()->GetMatchState() == MatchState::InProgress)
				{
					//TODO: 사망한 상태에서 캐릭터를 변경하는 경우 즉시 부활하는 버그를 유발합니다.
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

	BaseGameState = GetWorld()->GetGameState<ALakayaBaseGameState>();
	if (BaseGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_OccupationGameState is null."));
	}

	CurrentPlayerNum = BaseGameState->PlayerArray.Num();

	// TODO : 플레이어가 다 모이게 되면 N초 (DelayedMatchStart)뒤에 MatchStart로 넘어가게 해야 합니다.
	// TODO : 여기서 진행하기 보다는 개인전과 별개로, OccupationGameMode에서 진행해야 합니다.
	// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
	// if (CurrentPlayerNum == BaseGameState->GetMaximumPlayers())
	// {
	// 	GetWorldTimerManager().SetTimer(TimerHandle_DelayedCharacterSelectStart, this, &ALakayaDefaultPlayGameMode::StartSelectCharacter,
	// 		CharacterSelectStartDelay, false);
	// }
}