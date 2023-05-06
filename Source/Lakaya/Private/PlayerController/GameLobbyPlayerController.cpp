#include "PlayerController/GameLobbyPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameMode/LakayaBaseGameState.h"


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

AGameLobbyPlayerController::AGameLobbyPlayerController()
{
	if (IsRunningDedicatedServer()) return;

	InterfaceContextPriority = 100;

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_InterfaceControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MenuFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Menu'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> WeaponFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Loadout'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ShowScoreFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Dev/Yongwoo/Input/IA_ShowScore.IA_ShowScore'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> HideScoreFinder(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Dev/Yongwoo/Input/IA_HideScore.IA_HideScore'"));

	if (ContextFinder.Succeeded()) InterfaceInputContext = ContextFinder.Object;
	if (MenuFinder.Succeeded()) MenuAction = MenuFinder.Object;
	if (WeaponFinder.Succeeded()) LoadoutAction = WeaponFinder.Object;
	if (ShowScoreFinder.Succeeded()) ShowScoreAction = ShowScoreFinder.Object;
	if (HideScoreFinder.Succeeded()) HideScoreAction = HideScoreFinder.Object;
}

void AGameLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	OnPossessedPawnChanged.AddUniqueDynamic(this, &AGameLobbyPlayerController::OnPossessedPawnChangedCallback);
}

void AGameLobbyPlayerController::MenuHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Menu"));
}

void AGameLobbyPlayerController::LoadoutHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("WeaponLoadout"));
}

void AGameLobbyPlayerController::ShowScoreBoard(const FInputActionValue& Value)
{
	if (const auto GameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
		GameState->SetScoreBoardVisibility(true);
}

void AGameLobbyPlayerController::HideScoreBoard(const FInputActionValue& Value)
{
	if (const auto GameState = GetWorld()->GetGameState<ALakayaBaseGameState>())
		GameState->SetScoreBoardVisibility(false);
}
