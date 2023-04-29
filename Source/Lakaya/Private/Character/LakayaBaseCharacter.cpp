// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Character/ResourceComponent.h"
#include "Character/StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

ALakayaBaseCharacter::ALakayaBaseCharacter()
{
	MaxHealth = 100.f;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleRadius());
	bUseControllerRotationYaw = bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("/Script/Engine.DataTable'/Game/Dev/Yongwoo/DataTables/DT_CharacterSetupTable.DT_CharacterSetupTable'"));

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

float ALakayaBaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	const auto LocalState = GetPlayerState();

	// 플레이어 스테이트가 없는 경우 원본의 로직을 실행합니다.
	if (!LocalState) return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 플레이어 스테이트에서 데미지를 처리하고나서, 애니메이션 재생을 위해 캐릭터에서도 데미지를 처리합니다.
	const auto Damage = LocalState->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

float ALakayaBaseCharacter::InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent,
                                                     AController* EventInstigator, AActor* DamageCauser)
{
	// 플레이어 스테이트가 존재하는 경우 이미 범위피해 보정이 이뤄졌을 것이므로, 여기서는 무시합니다.
	if (GetPlayerState()) return Damage;
	return Super::InternalTakeRadialDamage(Damage, RadialDamageEvent, EventInstigator, DamageCauser);
}

void ALakayaBaseCharacter::OnSetTeam(const EPlayerTeam& Team)
{
	
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
