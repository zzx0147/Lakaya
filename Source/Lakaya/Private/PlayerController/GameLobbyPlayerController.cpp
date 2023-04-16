#include "PlayerController/GameLobbyPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


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
	EnhancedInputComponent->BindAction(ScoreAction, ETriggerEvent::Triggered, this,
	                                   &AGameLobbyPlayerController::ScoreHandler);
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

	static const ConstructorHelpers::FObjectFinder<UInputAction> ScoreFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ScoreBoard'"));

	if (ContextFinder.Succeeded()) InterfaceInputContext = ContextFinder.Object;
	if (MenuFinder.Succeeded()) MenuAction = MenuFinder.Object;
	if (WeaponFinder.Succeeded()) LoadoutAction = WeaponFinder.Object;
	if (ScoreFinder.Succeeded()) ScoreAction = ScoreFinder.Object;
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

void AGameLobbyPlayerController::ScoreHandler(const FInputActionValue& Value)
{
	//TODO: UI를 띄웁니다.
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Score"));
}
