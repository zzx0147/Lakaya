// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AttachableProjectile.h"

#include "Net/UnrealNetwork.h"

void AAttachableProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAttachableProjectile, TargetActor);
	DOREPLIFETIME(AAttachableProjectile, AttachedBone);
}

AAttachableProjectile::AAttachableProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	bAutoEnding = bHideMeshOnEnding = false;
	ActionDelay = 0.4f;
}

void AAttachableProjectile::OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                             AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                             int32 OtherBodyIndex, bool bFromSweep,
                                                             const FHitResult& SweepResult)
{
	const auto Start = OverlappedComponent->GetComponentLocation();
	const auto Velocity = OverlappedComponent->GetPhysicsLinearVelocity();
	Super::OnCollisionComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
	                                        SweepResult);
	TargetActor = OtherActor;

	// 투사체가 진행중에 충돌한 경우 간단히 SweepResult를 사용합니다.
	if (bFromSweep)
	{
		AttachedBone = SweepResult.BoneName;
		SetActorLocationAndRotation(SweepResult.ImpactPoint, SweepResult.ImpactNormal.Rotation());
	}
	// SweepResult를 사용할 수 없는 경우 원래 투사체가 진행하던 방향으로 조금 더 진행시켜서 부착될 위치를 찾습니다.
	else if (FHitResult HitResult;
		OtherComp->SweepComponent(HitResult, Start, Start + Velocity * 100.f,
		                          OverlappedComponent->GetComponentRotation().Quaternion(),
		                          OverlappedComponent->GetCollisionShape(), true))
	{
		AttachedBone = HitResult.BoneName;
		SetActorLocationAndRotation(HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
	// 충돌에 실패한 경우 그냥 투사체가 최초로 충돌했던 위치로 두고, 진행하던 방향의 반대 방향을 바라보도록 셋팅합니다.
	else
	{
		AttachedBone = NAME_None;
		SetActorLocationAndRotation(Start, (-Velocity).Rotation());
	}

	// ALinearProjectile에서 ProjectileLocation은 월드좌표계였지만, 여기서는 부모 액터가 계속 움직이고 있을 수 있으므로 상대좌표계를 사용합니다.
	AttachToActor(TargetActor, FAttachmentTransformRules::KeepWorldTransform, AttachedBone);
	ProjectileLocation = GetRootComponent()->GetRelativeLocation();
	ProjectileRotation = GetRootComponent()->GetRelativeRotation();
	SetAbilityInstanceState(EAbilityInstanceState::ReadyForAction);
}

void AAttachableProjectile::HandleAbilityInstanceReadyForAction()
{
	Super::HandleAbilityInstanceReadyForAction();
	if (HasAuthority()) return;
	// ALinearProjectile에서 ProjectileLocation은 월드좌표계였지만, 여기서는 부모 액터가 계속 움직이고 있을 수 있으므로 상대좌표계를 사용합니다.
	AttachToActor(TargetActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachedBone);
	SetActorRelativeLocation(ProjectileLocation);
	SetActorRelativeRotation(ProjectileRotation);
}

void AAttachableProjectile::HandleAbilityInstanceAction()
{
	Super::HandleAbilityInstanceAction();
	if (HasAuthority()) return;
	// ALinearProjectile에서 ProjectileLocation은 월드좌표계였지만, 여기서는 부모 액터가 계속 움직이고 있을 수 있으므로 상대좌표계를 사용합니다.
	AttachToActor(TargetActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachedBone);
	SetActorRelativeLocation(ProjectileLocation);
	SetActorRelativeRotation(ProjectileRotation);
}

void AAttachableProjectile::HandleAbilityInstanceCollapsed()
{
	Super::HandleAbilityInstanceCollapsed();
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}
