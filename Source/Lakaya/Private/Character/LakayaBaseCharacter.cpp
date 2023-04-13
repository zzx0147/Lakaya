// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Character/ResourceComponent.h"
#include "Character/StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

ALakayaBaseCharacter::ALakayaBaseCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleRadius());
	bUseControllerRotationYaw = bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	//TODO: Path 지정
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(TEXT("DataTable''"));
	if (TableFinder.Succeeded()) CharacterSetupTable = TableFinder.Object;
}

ELifetimeCondition ALakayaBaseCharacter::AllowActorComponentToReplicate(
	const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UResourceComponent::StaticClass()) ||
		ComponentToReplicate->IsA(UStatComponent::StaticClass()))
		return COND_None;
	return Super::AllowActorComponentToReplicate(ComponentToReplicate);
}

void ALakayaBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALakayaBaseCharacter, CharacterName);
	DOREPLIFETIME(ALakayaBaseCharacter, ResourceComponent);
	DOREPLIFETIME(ALakayaBaseCharacter, StatComponent);
}

void ALakayaBaseCharacter::SetupCharacter(const FName& RowName)
{
	const auto Data = CharacterSetupTable->FindRow<FCharacterSetupData>(RowName,TEXT("SetupCharacter"));
	if (!Data) return;

	CharacterName = RowName;
	SetupCharacterServer(Data);
	OnCharacterNameChanged.Broadcast(CharacterName);
}

void ALakayaBaseCharacter::OnRep_CharacterName()
{
	const auto Data = CharacterSetupTable->FindRow<FCharacterSetupData>(CharacterName,TEXT("OnRep_CharacterName"));
	if (!Data) return;

	SetupCharacterClient(Data);
	OnCharacterNameChanged.Broadcast(CharacterName);
}

void ALakayaBaseCharacter::OnRep_ResourceComponent()
{
	OnResourceChanged.Broadcast(ResourceComponent);
}

void ALakayaBaseCharacter::OnRep_StatComponent()
{
	OnStatChanged.Broadcast(StatComponent);
}

void ALakayaBaseCharacter::SetupCharacterServer(const FCharacterSetupData* Data)
{
	ResourceComponent = CreateReplicatedComponent(Data->ResourceClass);
	OnResourceChanged.Broadcast(ResourceComponent);

	StatComponent = CreateReplicatedComponent(Data->StatClass);
	StatComponent->SetupStat(CharacterName);
	OnStatChanged.Broadcast(StatComponent);

	SetupMeshActor(Data->MeshActorClass);
}

void ALakayaBaseCharacter::SetupCharacterClient(const FCharacterSetupData* Data)
{
	SetupMeshActor(Data->MeshActorClass);
}

void ALakayaBaseCharacter::SetupMeshActor(const TSubclassOf<AActor>& ActorClass)
{
	const auto SpawnedActor = GetWorld()->SpawnActor(ActorClass);
	SpawnedActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
}
