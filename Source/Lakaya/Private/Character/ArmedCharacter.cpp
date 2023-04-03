// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ArmedCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Weapon/WeaponAbility.h"
#include "Weapon/WeaponClassData.h"
#include "Weapon/WeaponComponent.h"
#include "Weapon/WeaponFire.h"
#include "Weapon/WeaponReload.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"

AArmedCharacter::AArmedCharacter()
{
	bReplicateUsingRegisteredSubObjectList = true;

	if (IsRunningDedicatedServer()) return;

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

	static const ConstructorHelpers::FObjectFinder<UDataTable> DataFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/DT_WeaponClassDataTable'"));

	if (ContextFinder.Succeeded()) WeaponControlContext = ContextFinder.Object;
	if (FireStartFinder.Succeeded()) FireStartAction = FireStartFinder.Object;
	if (FireStopFinder.Succeeded()) FireStopAction = FireStopFinder.Object;
	if (AbilityStartFinder.Succeeded()) AbilityStartAction = AbilityStartFinder.Object;
	if (AbilityStopFinder.Succeeded()) AbilityStopAction = AbilityStopFinder.Object;
	if (ReloadStartFinder.Succeeded()) ReloadStartAction = ReloadStartFinder.Object;
	if (ReloadStopFinder.Succeeded()) ReloadStopAction = ReloadStopFinder.Object;
	if (DataFinder.Succeeded()) WeaponClassDataTable = DataFinder.Object;
}

void AArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArmedCharacter, PrimaryWeapon);
}

void AArmedCharacter::SetupPrimaryWeapon(const FName& WeaponClassRowName)
{
	auto Data = WeaponClassDataTable->FindRow<FWeaponClassData>(WeaponClassRowName,TEXT("SetupPrimaryWeapon"));

	PrimaryWeapon = Cast<UWeaponComponent>(
		AddComponentByClass(Data->WeaponClass.LoadSynchronous(), false, FTransform::Identity, false));

	if (!PrimaryWeapon) UE_LOG(LogActor, Fatal, TEXT("PrimaryWeapon was setted as nullptr"));
	PrimaryWeapon->RequestSetupData(Data->AssetRowName);
	PrimaryWeapon->SetIsReplicated(true);
	if (!PrimaryWeapon->GetIsReplicated()) UE_LOG(LogTemp, Fatal, TEXT("PrimaryWeapon is NOT replicated"));
}

void AArmedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto CastedComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		CastedComponent->BindAction(FireStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::FireStart);
		CastedComponent->BindAction(FireStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::FireStop);
		CastedComponent->BindAction(AbilityStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::AbilityStart);
		CastedComponent->BindAction(AbilityStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::AbilityStop);
		CastedComponent->BindAction(ReloadStartAction, ETriggerEvent::Triggered, this, &AArmedCharacter::ReloadStart);
		CastedComponent->BindAction(ReloadStopAction, ETriggerEvent::Triggered, this, &AArmedCharacter::ReloadStop);
	}
}

ELifetimeCondition AArmedCharacter::AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UWeaponComponent::StaticClass())) return COND_None;
	return Super::AllowActorComponentToReplicate(ComponentToReplicate);
}

void AArmedCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!GetIsReplicated()) UE_LOG(LogTemp, Fatal, TEXT("ArmedCharacter is NOT replicated"));
	if (HasAuthority()) SetupPrimaryWeapon(TEXT("Test"));
	AddInputContext();
}

void AArmedCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacter(EventInstigator, DamageCauser);
	PrimaryWeapon->UpgradeInitialize();
	auto Causer = Cast<AArmedCharacter>(DamageCauser);
	if (Causer) Causer->PrimaryWeapon->UpgradeWeapon();
}

void AArmedCharacter::KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacterNotify_Implementation(EventInstigator, DamageCauser);
	RemoveInputContext();
}

void AArmedCharacter::RespawnNotify_Implementation()
{
	Super::RespawnNotify_Implementation();
	AddInputContext();
}

void AArmedCharacter::CallBeginPlay()
{
	BeginPlay();
}

void AArmedCharacter::FireStart(const FInputActionValue& Value)
{
	PrimaryWeapon->FireStart();
}

void AArmedCharacter::FireStop(const FInputActionValue& Value)
{
	PrimaryWeapon->FireStop();
}

void AArmedCharacter::AbilityStart(const FInputActionValue& Value)
{
	PrimaryWeapon->AbilityStart();
}

void AArmedCharacter::AbilityStop(const FInputActionValue& Value)
{
	PrimaryWeapon->AbilityStop();
}

void AArmedCharacter::ReloadStart(const FInputActionValue& Value)
{
	PrimaryWeapon->ReloadStart();
}

void AArmedCharacter::ReloadStop(const FInputActionValue& Value)
{
	PrimaryWeapon->ReloadStop();
}
