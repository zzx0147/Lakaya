#include "PlayerController/GameLobbyPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/LakayaBaseGameState.h"
#include "GameMode/OccupationGameState.h"
#include "Input/LakayaInputContext.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "Kismet/GameplayStatics.h"


void AGameLobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const auto CastedComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(CastedComponent)
	SetupEnhancedInputComponent(CastedComponent);

	const auto InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSubsystem)
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

void AGameLobbyPlayerController::SetEnableExitShortcut(const bool& Enable)
{
	bEnableExitShortcut = Enable;
}

void AGameLobbyPlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::MenuHandler);
	EnhancedInputComponent->BindAction(ShowScoreAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::ShowScoreBoard);
	EnhancedInputComponent->BindAction(HideScoreAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::HideScoreBoard);

	if (!AbilityInputSet.IsNull())
	{
		AbilityInputSet.LoadSynchronous()->BindActions(EnhancedInputComponent, this,
		                                               &AGameLobbyPlayerController::AbilityInput, InputHandleContainer);
	}
}

void AGameLobbyPlayerController::UnbindAllAndBindMenu(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	EnhancedInputComponent->ClearActionBindings();
	EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::MenuHandler);
	EnhancedInputComponent->BindAction(ShowScoreAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::ShowScoreBoard);
	EnhancedInputComponent->BindAction(HideScoreAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::HideScoreBoard);
}

void AGameLobbyPlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	InputSubsystem->AddMappingContext(InterfaceInputContext, InterfaceContextPriority);
	if (!AbilityInputContext.IsNull())
	{
		AbilityInputContext.LoadSynchronous()->AddMappingContext(InputSubsystem);
	}
}

AGameLobbyPlayerController::AGameLobbyPlayerController(): APlayerController()
{
	OnPossessedPawnChanged.AddUniqueDynamic(this, &AGameLobbyPlayerController::OnPossessedPawnChangedCallback);
	if (IsRunningDedicatedServer()) return;

	InterfaceContextPriority = 100;

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Input/IC_InterfaceControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MenuFinder(
		TEXT("InputAction'/Game/Input/IA_Menu'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ShowScoreFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ShowScore.IA_ShowScore'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> HideScoreFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_HideScore.IA_HideScore'"));

	if (ContextFinder.Succeeded()) InterfaceInputContext = ContextFinder.Object;
	if (MenuFinder.Succeeded()) MenuAction = MenuFinder.Object;
	if (ShowScoreFinder.Succeeded()) ShowScoreAction = ShowScoreFinder.Object;
	if (HideScoreFinder.Succeeded()) HideScoreAction = HideScoreFinder.Object;

	ExitLevel = FSoftObjectPath(TEXT("/Script/Engine.World'/Game/Levels/MainLobbyLevel.MainLobbyLevel'"));
}

UAbilitySystemComponent* AGameLobbyPlayerController::GetAbilitySystemComponent() const
{
	if (AbilitySystem.IsValid()) return AbilitySystem.Get();
	const auto CastedState = GetPlayerState<IAbilitySystemInterface>();
	return ensure(CastedState) ? CastedState->GetAbilitySystemComponent() : nullptr;
}

void AGameLobbyPlayerController::MenuHandler()
{
	if (bEnableExitShortcut) UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), ExitLevel);
}

void AGameLobbyPlayerController::AbilityInput(TAbilitySystemInputCallback Function, int32 InputID)
{
	if (!AbilitySystem.IsValid())
	{
		AbilitySystem = GetAbilitySystemComponent();
		if (!ensure(AbilitySystem.IsValid())) return;
	}
	(AbilitySystem.Get()->*Function)(InputID);
}

void AGameLobbyPlayerController::ShowScoreBoard()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowScoreBoard"));

	// 팀전일 때
	if (const auto OccupationGameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
	{
		if (const auto NewGameState = Cast<AOccupationGameState>(OccupationGameState))
		{
			if (!NewGameState->Tapbool) return;

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

void AGameLobbyPlayerController::HideScoreBoard()
{
	if (const auto GameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
		GameState->SetScoreBoardVisibility(false);
}
