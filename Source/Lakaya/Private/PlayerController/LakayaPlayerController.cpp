#include "PlayerController/LakayaPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/AIIndividualGameState.h"
#include "GameMode/LakayaBaseGameState.h"
#include "GameMode/OccupationGameState.h"
#include "Input/LakayaInputContext.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "Kismet/GameplayStatics.h"


void ALakayaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const auto CastedComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(CastedComponent)
	SetupEnhancedInputComponent(CastedComponent);

	const auto InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSubsystem)
	SetupMappingContext(InputSubsystem);
}

void ALakayaPlayerController::OnPossess(APawn* PawnToPossess)
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

void ALakayaPlayerController::SetEnableExitShortcut(const bool& Enable)
{
	bEnableExitShortcut = Enable;
}

void ALakayaPlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this,
	                                   &ALakayaPlayerController::MenuHandler);
	EnhancedInputComponent->BindAction(ShowScoreAction, ETriggerEvent::Triggered, this,
	                                   &ALakayaPlayerController::ShowScoreBoardAndTabMinimap);
	EnhancedInputComponent->BindAction(HideScoreAction, ETriggerEvent::Triggered, this,
	                                   &ALakayaPlayerController::HideScoreBoardAndTabMinimap);

	if (!AbilityInputSet.IsNull())
	{
		AbilityInputSet.LoadSynchronous()->BindActions(EnhancedInputComponent, this,
		                                               &ALakayaPlayerController::AbilityInput, InputHandleContainer);
	}
}

void ALakayaPlayerController::UnbindAllAndBindMenu(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	EnhancedInputComponent->ClearActionBindings();
	EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this,
	                                   &ALakayaPlayerController::MenuHandler);
	EnhancedInputComponent->BindAction(ShowScoreAction, ETriggerEvent::Triggered, this,
	                                   &ALakayaPlayerController::ShowScoreBoardAndTabMinimap);
	EnhancedInputComponent->BindAction(HideScoreAction, ETriggerEvent::Triggered, this,
	                                   &ALakayaPlayerController::HideScoreBoardAndTabMinimap);
}

void ALakayaPlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	InputSubsystem->AddMappingContext(InterfaceInputContext, InterfaceContextPriority);
	if (!AbilityInputContext.IsNull())
	{
		AbilityInputContext.LoadSynchronous()->AddMappingContext(InputSubsystem);
	}
}

ALakayaPlayerController::ALakayaPlayerController()
{
	OnPossessedPawnChanged.AddUniqueDynamic(this, &ALakayaPlayerController::OnPossessedPawnChangedCallback);
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

UAbilitySystemComponent* ALakayaPlayerController::GetAbilitySystemComponent() const
{
	if (AbilitySystem.IsValid()) return AbilitySystem.Get();
	const auto CastedState = GetPlayerState<IAbilitySystemInterface>();
	return ensure(CastedState) ? CastedState->GetAbilitySystemComponent() : nullptr;
}

void ALakayaPlayerController::MenuHandler()
{
	if (bEnableExitShortcut) UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), ExitLevel);
}

void ALakayaPlayerController::AbilityInput(TAbilitySystemInputCallback Function, int32 InputID)
{
	if (!AbilitySystem.IsValid())
	{
		AbilitySystem = GetAbilitySystemComponent();
		if (!ensure(AbilitySystem.IsValid())) return;
	}
	(AbilitySystem.Get()->*Function)(InputID);
}

void ALakayaPlayerController::ShowScoreBoardAndTabMinimap()
{
	// 팀전일 때
	if (const auto& NewGameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
	{
		if (const auto OccupationGameState = Cast<AOccupationGameState>(NewGameState))
		{
			if (OccupationGameState == nullptr) UE_LOG(LogTemp, Warning, TEXT("GameLobbyPlayerController_OccupationGameState is null."));

			OccupationGameState->SetScoreBoardVisibility(true);
			OccupationGameState->SetTabMinimapVisibility(true);
		}
	}

	// TODO: 개인전에서의 스코어보드를 작성해야 합니다.
	// 개인전일 때
	if (const auto& NewGameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
	{
		if (const auto AIIndividualGameState = Cast<AAIIndividualGameState>(NewGameState))
		{
			if (AIIndividualGameState == nullptr) UE_LOG(LogTemp, Warning, TEXT("GameLobbyPlayerController_AIIndividualGameState is null."));

			AIIndividualGameState->SetScoreBoardVisibility(true);
			AIIndividualGameState->SetTabMinimapVisibility(true);
		}
	}
}

void ALakayaPlayerController::HideScoreBoardAndTabMinimap()
{
	// 팀전
	if (const auto& NewGameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
	{
		if (const auto& OccupationGameState = Cast<AOccupationGameState>(NewGameState))
		{
			OccupationGameState->SetScoreBoardVisibility(false);
			OccupationGameState->SetTabMinimapVisibility(false);
		}
	}
	// 개인전
	if (const auto& NewGameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
	{
		if (const auto& AIIndividualGameState = Cast<AAIIndividualGameState>(NewGameState))
		{
			AIIndividualGameState->SetScoreBoardVisibility(false);
			AIIndividualGameState->SetTabMinimapVisibility(false);
		}
	}
}