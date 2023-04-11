// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ArmedCharacter.h"

#include "Character/CharacterSetupData.h"
#include "Character/ResourceComponent.h"
#include "Character/Ability/CharacterAbility.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"

void AArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArmedCharacter, Abilities);
	DOREPLIFETIME(AArmedCharacter, ResourceComponent);
}

AArmedCharacter::AArmedCharacter()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(TEXT("DataTable''"));
	if (TableFinder.Succeeded()) CharacterSetupTable = TableFinder.Object;
}

ELifetimeCondition AArmedCharacter::AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UCharacterAbility::StaticClass()) ||
		ComponentToReplicate->IsA(UResourceComponent::StaticClass()))
		return COND_None;

	return Super::AllowActorComponentToReplicate(ComponentToReplicate);
}

void AArmedCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AArmedCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacter(EventInstigator, DamageCauser);
}

void AArmedCharacter::KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacterNotify_Implementation(EventInstigator, DamageCauser);
}

void AArmedCharacter::RespawnNotify_Implementation()
{
	Super::RespawnNotify_Implementation();
}

void AArmedCharacter::SetupCharacter(const FName& RowName)
{
	const auto Data = CharacterSetupTable->FindRow<FCharacterSetupData>(RowName,TEXT("SetupCharacter"));
	if (!Data) UE_LOG(LogInit, Fatal, TEXT("Fail to load character setup data!"));

	CharacterName = RowName;
	OnCharacterNameChanged.Broadcast(CharacterName);

	Abilities.Reserve(5);
	Abilities.EmplaceAt(Primary, CreateReplicatedComponent(Data->PrimaryClass));
	Abilities.EmplaceAt(Secondary, CreateReplicatedComponent(Data->SecondClass));
	Abilities.EmplaceAt(WeaponFire, CreateReplicatedComponent(Data->FireClass));
	Abilities.EmplaceAt(WeaponAbility, CreateReplicatedComponent(Data->AbilityClass));
	Abilities.EmplaceAt(WeaponReload, CreateReplicatedComponent(Data->ReloadClass));
	OnAbilitiesChanged.Broadcast(Abilities);

	ResourceComponent = CreateReplicatedComponent(Data->ResourceClass);
	OnResourceChanged.Broadcast(ResourceComponent);

	SetupMeshActor(Data->MeshActorClass);
}

void AArmedCharacter::StartAbility(const EAbilityKind& Kind)
{
	if (Abilities.Contains(Kind)) Abilities[Kind]->AbilityStart();
}

void AArmedCharacter::StopAbility(const EAbilityKind& Kind)
{
	if (Abilities.Contains(Kind)) Abilities[Kind]->AbilityStop();
}

void AArmedCharacter::OnRep_Abilities()
{
	OnAbilitiesChanged.Broadcast(Abilities);
}

void AArmedCharacter::OnRep_ResourceComponent()
{
	OnResourceChanged.Broadcast(ResourceComponent);
}

void AArmedCharacter::OnRep_CharacterName()
{
	const auto Data = CharacterSetupTable->FindRow<FCharacterSetupData>(CharacterName,TEXT("SetupCharacter"));
	if (!Data) UE_LOG(LogInit, Fatal, TEXT("Fail to load character setup data!"));

	SetupMeshActor(Data->MeshActorClass);
	OnCharacterNameChanged.Broadcast(CharacterName);
}

void AArmedCharacter::SetupMeshActor(const TSubclassOf<AActor>& ActorClass)
{
	//TODO: 메시 액터 클래스를 가져와서 생성하고, 자식으로 추가합니다.
}
