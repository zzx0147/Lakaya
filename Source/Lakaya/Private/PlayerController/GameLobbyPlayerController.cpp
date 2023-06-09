#include "PlayerController/GameLobbyPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/AIIndividualGameState.h"
#include "GameMode/LakayaBaseGameState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "GameMode/OccupationGameState.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "Kismet/GameplayStatics.h"


void AGameLobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const auto CastedComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!CastedComponent) UE_LOG(LogInit, Fatal, TEXT("InputComponent was not UEnhancedInputComponent!"));
	SetupEnhancedInputComponent(CastedComponent);

	const auto InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem) UE_LOG(LogInit, Fatal, TEXT("UEnhancedInputLocalPlayerSubsystem was not exist!"));
	SetupMappingContext(InputSubsystem);
}

void AGameLobbyPlayerController::OnPossess(APawn* PawnToPossess)
{
	if (PawnToPossess != NULL &&
		(PlayerState == NULL || !PlayerState->IsOnlyASpectator()))
	{
		const bool bNewPawn = (GetPawn() != PawnToPossess);

		if (GetPawn() && bNewPawn)
		{
			UnPossess();
		}

		if (PawnToPossess->Controller != NULL && PawnToPossess->Controller != this) //이 컨트롤러가 나랑 같으면 하면 안됨 조건 추가
		{
			PawnToPossess->Controller->UnPossess();
		}

		PawnToPossess->PossessedBy(this);

		// update rotation to match possessed pawn's rotation
		SetControlRotation(PawnToPossess->GetActorRotation());

		SetPawn(PawnToPossess);
		check(GetPawn() != NULL);

		if (GetPawn() && GetPawn()->PrimaryActorTick.bStartWithTickEnabled)
		{
			GetPawn()->SetActorTickEnabled(true);
		}

		INetworkPredictionInterface* NetworkPredictionInterface =
			GetPawn() ? Cast<INetworkPredictionInterface>(GetPawn()->GetMovementComponent()) : NULL;
		if (NetworkPredictionInterface)
		{
			NetworkPredictionInterface->ResetPredictionData_Server();
		}

		AcknowledgedPawn = NULL;

		// Local PCs will have the Restart() triggered right away in ClientRestart (via PawnClientRestart()), but the server should call Restart() locally for remote PCs.
		// We're really just trying to avoid calling Restart() multiple times.
		if (!IsLocalPlayerController())
		{
			GetPawn()->DispatchRestart(false);
		}

		ClientRestart(GetPawn());

		ChangeState(NAME_Playing);
		if (bAutoManageActiveCameraTarget)
		{
			AutoManageActiveCameraTarget(GetPawn());
			ResetCameraMode();
		}
	}
}

void AGameLobbyPlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::MenuHandler);
	EnhancedInputComponent->BindAction(LoadoutAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::LoadoutHandler);
	EnhancedInputComponent->BindAction(ShowScoreAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::ShowScoreBoard);
	EnhancedInputComponent->BindAction(HideScoreAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::HideScoreBoard);
}

void AGameLobbyPlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	InputSubsystem->AddMappingContext(InterfaceInputContext, InterfaceContextPriority);
}

void AGameLobbyPlayerController::NotifyLocalPlayerStateUpdated()
{
	if (const auto GameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
		GameState->OnLocalPlayerControllerPlayerStateUpdated(this);
}

AGameLobbyPlayerController::AGameLobbyPlayerController()
{
	OnPossessedPawnChanged.AddUniqueDynamic(this, &AGameLobbyPlayerController::OnPossessedPawnChangedCallback);
	if (IsRunningDedicatedServer()) return;

	InterfaceContextPriority = 100;

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Input/IC_InterfaceControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MenuFinder(
		TEXT("InputAction'/Game/Input/IA_Menu'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> WeaponFinder(
		TEXT("InputAction'/Game/Input/IA_Loadout'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ShowScoreFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ShowScore.IA_ShowScore'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> HideScoreFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_HideScore.IA_HideScore'"));

	if (ContextFinder.Succeeded()) InterfaceInputContext = ContextFinder.Object;
	if (MenuFinder.Succeeded()) MenuAction = MenuFinder.Object;
	if (WeaponFinder.Succeeded()) LoadoutAction = WeaponFinder.Object;
	if (ShowScoreFinder.Succeeded()) ShowScoreAction = ShowScoreFinder.Object;
	if (HideScoreFinder.Succeeded()) HideScoreAction = HideScoreFinder.Object;
}

void AGameLobbyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//클라의 경우 PlayerState가 생겼을 때 캐릭터 선택 위젯을 생성
	if (IsLocalController()) NotifyLocalPlayerStateUpdated();
}

void AGameLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//서버의 경우에만 BeginPlay에서 캐릭터 선택 위젯을 생성
	if (HasAuthority() && IsLocalController()) NotifyLocalPlayerStateUpdated();
}

void AGameLobbyPlayerController::MenuHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Tab Key."));

	if (GetWorld()->GetGameState<ALakayaBaseGameState>()->GetMatchState() == MatchState::WaitingPostMatch)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("뒤로가기"));
		UGameplayStatics::OpenLevel(GetWorld(), "MainLobbyLevel");
	}
}

void AGameLobbyPlayerController::LoadoutHandler(const FInputActionValue& Value)
{
	if (const auto GameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
		GameState->ToggleCharacterSelectWidget();
}

void AGameLobbyPlayerController::ShowScoreBoard(const FInputActionValue& Value)
{
	// 팀전일 때
	if (const auto OccupationGameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
	{
		if (const auto NewGameState = Cast<AOccupationGameState>(OccupationGameState))
		{
			if (!NewGameState->TapBool) return;

			if (OccupationGameState->GetMatchState() == MatchState::WaitingPostMatch)
			{
				// 게임이 종료되고 결과창에 어떠한 위젯이 띄워지고 있느냐에 따라서 위젯들이 보여지는게 달라집니다.
				NewGameState->ChangeResultWidget();
			}

			OccupationGameState->SetScoreBoardVisibility(true);
		}
	}

	// 개인전일 때
}

void AGameLobbyPlayerController::HideScoreBoard(const FInputActionValue& Value)
{
	if (const auto GameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
	{
		if (GameState->GetMatchState() == MatchState::WaitingPostMatch) return;

		GameState->SetScoreBoardVisibility(false);
	}
}

void AGameLobbyPlayerController::EscapeHandler()
{
	UE_LOG(LogTemp, Warning, TEXT("EscapeHandler !"));	
}