// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Character/ResourceComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ALakayaBaseCharacter::ALakayaBaseCharacter()
{
	MaxHealth = 100.f;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	PrimaryActorTick.bCanEverTick = true;
	PlayerRotationInterpolationAlpha = 0.5f;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = bUseControllerRotationPitch = bUseControllerRotationRoll = false;
}

ELifetimeCondition ALakayaBaseCharacter::AllowActorComponentToReplicate(
	const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UResourceComponent::StaticClass())) return COND_None;
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
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ALakayaBaseCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	if (HasAuthority() && ResourceClass)
	{
		ResourceComponent = Cast<UResourceComponent>(
			AddComponentByClass(ResourceClass, false, FTransform::Identity, false));
		if (ResourceComponent) ResourceComponent->SetIsReplicated(true);
	}
}

float ALakayaBaseCharacter::InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent,
                                                     AController* EventInstigator, AActor* DamageCauser)
{
	// 플레이어 스테이트가 존재하는 경우 이미 범위피해 보정이 이뤄졌을 것이므로, 여기서는 무시합니다.
	if (GetPlayerState()) return Damage;
	return Super::InternalTakeRadialDamage(Damage, RadialDamageEvent, EventInstigator, DamageCauser);
}

void ALakayaBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 서버에서는 계속해서 플레이어의 회전정보를 업데이트합니다.
	if (HasAuthority())
	{
		PlayerRotation.Rotation = GetCamera()->GetComponentRotation().Quaternion();
		PlayerRotation.Time = GetServerTime();
	}

	// 시뮬레이트되는 클라이언트에서는 계속해서 플레이어의 회전을 외삽합니다. 
	else if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		const auto CurrentTime = GetServerTime();
		LatestUpdateRotation = FQuat::Slerp(LatestUpdateRotation, GetRawExtrapolatedRotator(CurrentTime),
		                                    PlayerRotationInterpolationAlpha);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LatestUpdateRotation.Vector() * 100.f,
		              FColor::Green, false, 0.3f);
	}
}

FRotator ALakayaBaseCharacter::GetPlayerRotation() const
{
	// 서버이거나 Autonomous인 경우 그냥 카메라 컴포넌트를 사용합니다.
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy) return GetCamera()->GetComponentRotation();
	return LatestUpdateRotation.Rotator();
}

float ALakayaBaseCharacter::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ALakayaBaseCharacter::OnRep_PlayerRotation()
{
	// 최신 주기를 이전 주기로 바꿔주고, 새로 들어온 PlayerRotation을 최신주기로 설정합니다.
	PrevPlayerRotation = LatestPlayerRotation;
	LatestPlayerRotation = PlayerRotation;
}

FQuat ALakayaBaseCharacter::GetRawExtrapolatedRotator(const float& CurrentTime) const
{
	return FQuat::Slerp(PrevPlayerRotation.Rotation, LatestPlayerRotation.Rotation,
	                    UKismetMathLibrary::NormalizeToRange(CurrentTime, PrevPlayerRotation.Time,
	                                                         LatestPlayerRotation.Time));
}

void ALakayaBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ALakayaBaseCharacter, ResourceComponent, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ALakayaBaseCharacter, PlayerRotation, COND_SkipOwner);
}
