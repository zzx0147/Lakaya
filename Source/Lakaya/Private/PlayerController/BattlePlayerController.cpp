// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/BattlePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Character/ArmedCharacter.h"
#include "PlayerController/CharacterBindWidgetData.h"
#include "UI/CharacterBindableWidget.h"


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

	static const ConstructorHelpers::FClassFinder<UCharacterBindableWidget> HealthFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayHealthWidget"));

	static const ConstructorHelpers::FClassFinder<UCharacterBindableWidget> ConsecutiveFinder(
		TEXT("/Game/Blueprints/UMG/WBP_GamePlayConsecutiveKillsWidget"));

	static const ConstructorHelpers::FClassFinder<UCharacterBindableWidget> DamageIndicatorFinder(
		TEXT("/Game/Blueprints/UMG/WBP_DirectionalDamageIndicator"));

	HealthWidgetClass = HealthFinder.Class;
	if (!HealthWidgetClass) UE_LOG(LogController, Fatal, TEXT("Fail to find HealthWidgetClass!"));

	ConsecutiveKillsWidgetClass = ConsecutiveFinder.Class;
	if (!ConsecutiveKillsWidgetClass) UE_LOG(LogController, Fatal, TEXT("Fail to find ConsecutiveKillsWidgetClass!"));

	DamageIndicatorClass = DamageIndicatorFinder.Class;
	if (!DamageIndicatorClass) UE_LOG(LogController, Fatal, TEXT("Fail to find DamageIndicatorClass!"));
}

void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	CharacterBindableWidgets.Reserve(3);
	CharacterBindableWidgets.Emplace(
		CreateViewportWidget<UCharacterBindableWidget>(HealthWidgetClass, ESlateVisibility::Hidden));
	CharacterBindableWidgets.Emplace(
		CreateViewportWidget<UCharacterBindableWidget>(ConsecutiveKillsWidgetClass, ESlateVisibility::Hidden));
	CharacterBindableWidgets.Emplace(
		CreateViewportWidget<UCharacterBindableWidget>(DamageIndicatorClass, ESlateVisibility::Hidden));
}

void ABattlePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	Super::SetupEnhancedInputComponent(EnhancedInputComponent);
	EnhancedInputComponent->BindAction(PrimaryStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::PrimaryStart);
	EnhancedInputComponent->BindAction(PrimaryStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::PrimaryStop);
	EnhancedInputComponent->BindAction(SecondStartAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::SecondStart);
	EnhancedInputComponent->BindAction(SecondStopAction, ETriggerEvent::Triggered, this,
	                                   &ABattlePlayerController::SecondStop);
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

	// 이전의 캐릭터에서 UI의 바인딩을 해제합니다.
	if (const auto CastedCharacter = Cast<ACharacter>(ArgOldPawn))
	{
		TArray<UCharacterBindableWidget*> WidgetsToRemove;
		WidgetsToRemove.Reserve(CharacterBindableWidgets.Num());

		// 제거 대상 위젯을 조사합니다.
		for (const auto& Widget : CharacterBindableWidgets)
			if (!Widget->UnbindCharacter(CastedCharacter))
				WidgetsToRemove.Emplace(Widget);

		// 제거되기를 희망하는 위젯들은 제거합니다.
		for (const auto& Widget : WidgetsToRemove)
		{
			Widget->RemoveFromParent();
			CharacterBindableWidgets.RemoveSwap(Widget, false);
		}
	}

	ArmedCharacter = Cast<AArmedCharacter>(NewPawn);
	if (ArmedCharacter.IsValid())
	{
		// 빙의 대상 캐릭터에 맞는 위젯을 생성하고 바인딩합니다.
		if (const auto Data = CharacterWidgetComponentTable->FindRow<FCharacterBindWidgetData>(
			ArmedCharacter->GetCharacterName(),TEXT("SetupCharacterWidgetComponent")))
		{
			CharacterBindableWidgets.Reserve(CharacterBindableWidgets.Num() + Data->WidgetList.Num());
			for (const auto& WidgetClass : Data->WidgetList)
			{
				auto Widget = CreateViewportWidget<UCharacterBindableWidget>(WidgetClass);
				Widget->BindCharacter(ArmedCharacter.Get());
				CharacterBindableWidgets.Emplace(Widget);
			}
		}
	}
}

void ABattlePlayerController::PrimaryStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(Primary);
}

void ABattlePlayerController::PrimaryStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(Primary);
}

void ABattlePlayerController::SecondStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(Secondary);
}

void ABattlePlayerController::SecondStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(Secondary);
}

void ABattlePlayerController::FireStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(WeaponFire);
}

void ABattlePlayerController::FireStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(WeaponFire);
}

void ABattlePlayerController::AbilityStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(WeaponAbility);
}

void ABattlePlayerController::AbilityStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(WeaponAbility);
}

void ABattlePlayerController::ReloadStart(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StartAbility(WeaponReload);
}

void ABattlePlayerController::ReloadStop(const FInputActionValue& Value)
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->StopAbility(WeaponReload);
}
