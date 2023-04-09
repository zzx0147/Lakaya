// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattlePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/ArmedCharacter.h"
#include "InputMappingContext.h"
#include "Character/DamageableCharacter.h"
#include "UI/GamePlayBulletWidget.h"
#include "UI/GamePlayConsecutiveKillsWidget.h"
#include "UI/GamePlayKillLogWidget.h"


ABattlePlayerController::ABattlePlayerController()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_WeaponControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_FireStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> FireStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_FireStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_AbilityStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> AbilityStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_AbilityStop'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ReloadStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> ReloadStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_ReloadStop'"));

	if (ContextFinder.Succeeded()) WeaponControlContext = ContextFinder.Object;
	if (FireStartFinder.Succeeded()) FireStartAction = FireStartFinder.Object;
	if (FireStopFinder.Succeeded()) FireStopAction = FireStopFinder.Object;
	if (AbilityStartFinder.Succeeded()) AbilityStartAction = AbilityStartFinder.Object;
	if (AbilityStopFinder.Succeeded()) AbilityStopAction = AbilityStopFinder.Object;
	if (ReloadStartFinder.Succeeded()) ReloadStartAction = ReloadStartFinder.Object;
	if (ReloadStopFinder.Succeeded()) ReloadStopAction = ReloadStopFinder.Object;

	static const ConstructorHelpers::FClassFinder<UGamePlayKillLogWidget> KillLogFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayKillLogWidget"));

	static const ConstructorHelpers::FClassFinder<UGamePlayHealthWidget> HealthFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayHealthWidget"));

	static const ConstructorHelpers::FClassFinder<UGamePlayBulletWidget> BulletFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayBulletWidget"));

	static const ConstructorHelpers::FClassFinder<UGamePlayConsecutiveKillsWidget> ConsecutiveFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayConsecutiveKillsWidget"));

	KillLogClass = KillLogFinder.Class;
	if (!KillLogClass) UE_LOG(LogController, Fatal, TEXT("Fail to find KillLogClass!"));

	HealthWidgetClass = HealthFinder.Class;
	if (!HealthWidgetClass) UE_LOG(LogController, Fatal, TEXT("Fail to find HealthWidgetClass!"));

	BulletWidgetClass = BulletFinder.Class;
	if (!BulletWidgetClass) UE_LOG(LogController, Fatal, TEXT("Fail to find BulletWidgetClass!"));

	ConsecutiveKillsWidgetClass = ConsecutiveFinder.Class;
	if (!ConsecutiveKillsWidgetClass) UE_LOG(LogController, Fatal, TEXT("Fail to find ConsecutiveKillsWidgetClass!"));
}

void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocalController()) return;

	if (const auto GameState = GetWorld()->GetGameState<AOccupationGameState>())
	{
		GameState->OnOccupationChangeGameState.AddLambda([this](EOccupationGameState State)
		{
			if (State == EOccupationGameState::Progress)
			{
				if (KillLogWidget) KillLogWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				if (HealthWidget) HealthWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				if (ConsecutiveKillsWidget)
					ConsecutiveKillsWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				if (BulletWidget) BulletWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		});
	}

	if (!KillLogWidget) KillLogWidget = CreateViewportWidget<UGamePlayKillLogWidget>(KillLogClass);
	if (!HealthWidget) HealthWidget = CreateViewportWidget<UGamePlayHealthWidget>(HealthWidgetClass);
	if (!ConsecutiveKillsWidget)
		ConsecutiveKillsWidget = CreateViewportWidget<UGamePlayConsecutiveKillsWidget>(ConsecutiveKillsWidgetClass);
	if (!BulletWidget) BulletWidget = CreateViewportWidget<UGamePlayBulletWidget>(BulletWidgetClass);
}

void ABattlePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	Super::SetupEnhancedInputComponent(EnhancedInputComponent);
	EnhancedInputComponent->BindAction(FireStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::FireStart);
	EnhancedInputComponent->BindAction(FireStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::FireStop);
	EnhancedInputComponent->BindAction(AbilityStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::AbilityStart);
	EnhancedInputComponent->BindAction(AbilityStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::AbilityStop);
	EnhancedInputComponent->BindAction(ReloadStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::ReloadStart);
	EnhancedInputComponent->BindAction(ReloadStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::ReloadStop);
}

void ABattlePlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	Super::SetupMappingContext(InputSubsystem);
	InputSubsystem->AddMappingContext(WeaponControlContext, WeaponContextPriority);
}

void ABattlePlayerController::OnPossessedPawnChangedCallback(APawn* ArgOldPawn, APawn* NewPawn)
{
	Super::OnPossessedPawnChangedCallback(ArgOldPawn, NewPawn);
	if (!IsLocalController()) return;
	ArmedCharacter = Cast<AArmedCharacter>(NewPawn);
	if (!ArmedCharacter.IsValid()) UE_LOG(LogInit, Warning, TEXT("NewPawn was not AAramedCharacter!"))
	
	if (!HealthWidget) HealthWidget = CreateViewportWidget<UGamePlayHealthWidget>(HealthWidgetClass);
	if (!ConsecutiveKillsWidget)
		ConsecutiveKillsWidget = CreateViewportWidget<UGamePlayConsecutiveKillsWidget>(ConsecutiveKillsWidgetClass);
	if (!BulletWidget) BulletWidget = CreateViewportWidget<UGamePlayBulletWidget>(BulletWidgetClass);

	HealthWidget->OnPossessedPawnChanged(ArgOldPawn, NewPawn);
	BulletWidget->OnPossessedPawnChanged(ArgOldPawn, NewPawn);
	ConsecutiveKillsWidget->OnPossessedPawnChanged(ArgOldPawn, NewPawn);
}

void ABattlePlayerController::OnCharacterBeginPlay(ACharacter* ArgCharacter)
{
	if (const auto Damageable = Cast<ADamageableCharacter>(ArgCharacter))
	{
		if (!KillLogWidget) KillLogWidget = CreateViewportWidget<UGamePlayKillLogWidget>(KillLogClass);
		KillLogWidget->OnCharacterBeginPlay(Damageable);
	}
}

void ABattlePlayerController::FireStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->FireStart();
}

void ABattlePlayerController::FireStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->FireStop();
}

void ABattlePlayerController::AbilityStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->AbilityStart();
}

void ABattlePlayerController::AbilityStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->AbilityStop();
}

void ABattlePlayerController::ReloadStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->ReloadStart();
}

void ABattlePlayerController::ReloadStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->ReloadStop();
}
