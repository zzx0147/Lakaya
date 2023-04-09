// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ArmedCharacter.h"

#include "Weapon/WeaponClassData.h"
#include "Weapon/WeaponComponent.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"

AArmedCharacter::AArmedCharacter()
{
	bReplicateUsingRegisteredSubObjectList = true;

	static const ConstructorHelpers::FObjectFinder<UDataTable> DataFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/DT_WeaponClassDataTable'"));

	if (DataFinder.Succeeded()) WeaponClassDataTable = DataFinder.Object;
}

void AArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArmedCharacter, PrimaryWeapon);
}

void AArmedCharacter::SetupPrimaryWeapon(const FName& WeaponClassRowName)
{
	const auto Data = WeaponClassDataTable->FindRow<FWeaponClassData>(WeaponClassRowName,TEXT("SetupPrimaryWeapon"));

	PrimaryWeapon = Cast<UWeaponComponent>(
		AddComponentByClass(Data->WeaponClass.LoadSynchronous(), false, FTransform::Identity, false));

	if (!PrimaryWeapon) UE_LOG(LogActor, Fatal, TEXT("PrimaryWeapon was setted as nullptr"));
	PrimaryWeapon->RequestSetupData(Data->AssetRowName);
	PrimaryWeapon->SetIsReplicated(true);
	if (!PrimaryWeapon->GetIsReplicated()) UE_LOG(LogTemp, Fatal, TEXT("PrimaryWeapon is NOT replicated"));
	OnPrimaryWeaponChanged.Broadcast(PrimaryWeapon);
}

ELifetimeCondition AArmedCharacter::AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UWeaponComponent::StaticClass())) return COND_None;
	return Super::AllowActorComponentToReplicate(ComponentToReplicate);
}

void AArmedCharacter::BeginPlay()
{
	Super::BeginPlay();

	//TODO: 무기 셋업은 게임모드에서 수행되어야 합니다.
	if (HasAuthority()) SetupPrimaryWeapon(TEXT("Test"));
}

void AArmedCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacter(EventInstigator, DamageCauser);
	PrimaryWeapon->UpgradeInitialize();
	if (const auto Causer = Cast<AArmedCharacter>(DamageCauser)) Causer->PrimaryWeapon->UpgradeWeapon();
}

void AArmedCharacter::KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacterNotify_Implementation(EventInstigator, DamageCauser);
	PrimaryWeapon->OnCharacterDead();
}

void AArmedCharacter::RespawnNotify_Implementation()
{
	Super::RespawnNotify_Implementation();
	PrimaryWeapon->OnCharacterRespawn();
}

void AArmedCharacter::FireStart()
{
	PrimaryWeapon->FireStart();
}

void AArmedCharacter::FireStop()
{
	PrimaryWeapon->FireStop();
}

void AArmedCharacter::AbilityStart()
{
	PrimaryWeapon->AbilityStart();
}

void AArmedCharacter::AbilityStop()
{
	PrimaryWeapon->AbilityStop();
}

void AArmedCharacter::ReloadStart()
{
	PrimaryWeapon->ReloadStart();
}

void AArmedCharacter::ReloadStop()
{
	PrimaryWeapon->ReloadStop();
}

void AArmedCharacter::OnRep_PrimaryWeapon()
{
	OnPrimaryWeaponChanged.Broadcast(PrimaryWeapon);
}
