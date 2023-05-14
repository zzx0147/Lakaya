// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/SpawnProjectileAbility.h"

#include "Character/Ability/LinearProjectile.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/SingleDamageProjectile.h"

USpawnProjectileAbility::USpawnProjectileAbility()
{
	bWantsInitializeComponent = true;
	ObjectPoolSize = 1;
	CoolTime = 5.f;
	SpawnDistance = 50.f;
}

void USpawnProjectileAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpawnProjectileAbility, EnableTime);
}

void USpawnProjectileAbility::AbilityStart()
{
	// 서버시간 예측의 오차와, 네트워크 딜레이를 감안하여 쿨타임이 돌기 조금 전부터 서버에 능력 사용요청을 할 수는 있도록 합니다.
	if (EnableTime - 0.1f <= GetServerTime()) Super::AbilityStart();
}

void USpawnProjectileAbility::InitializeComponent()
{
	Super::InitializeComponent();

	// 서버에서는 오브젝트풀을 세팅합니다.
	// if (!GetOwner()->HasAuthority()) return;
	// ProjectilePool.SetupObjectPool(ObjectPoolSize, [this]()-> ASingleDamageProjectile* {
	// 	FActorSpawnParameters Params;
	// 	Params.Instigator = GetOwner<APawn>();
	// 	Params.Owner = Params.Instigator ? Params.Instigator->GetController() : nullptr;
	// 	const auto Projectile = GetWorld()->SpawnActor<ASingleDamageProjectile>(ProjectileClass, Params);
	//
	// 	if (!Projectile)
	// 	{
	// 		UE_LOG(LogScript, Error, TEXT("Fail to spawn projectile!"));
	// 		return nullptr;
	// 	}
	//
	// 	Projectile->SetReplicates(true);
	// 	Projectile->DisableProjectile();
	// 	Projectile->OnAttackEnded.AddUObject(this, &USpawnProjectileAbility::OnProjectileReturned);
	// 	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Projectile spawned"));
	// 	return Projectile;
	// });
}

void USpawnProjectileAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);
	if (EnableTime > GetServerTime()) return;

	// if (const auto Projectile = ProjectilePool.GetObject())
	// {
	// 	Projectile->SetActorLocationAndRotation(
	// 		GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * SpawnDistance,
	// 		GetOwner()->GetActorRotation());
	// 	Projectile->EnableProjectile();
	// }

	if (const auto Projectile = GetWorld()->SpawnActor<ALinearProjectile>(LinearProjectileClass,
	                                                                      GetOwner()->GetActorLocation() + GetOwner()->
	                                                                      GetActorForwardVector() * SpawnDistance,
	                                                                      GetOwner()->GetActorRotation()))
	{
		Projectile->SetLifeSpan(5.f);
	}

	EnableTime = GetServerTime() + CoolTime;
	OnEnableTimeChanged.Broadcast(EnableTime);
}

void USpawnProjectileAbility::OnRep_EnableTime()
{
	OnEnableTimeChanged.Broadcast(EnableTime);
}

void USpawnProjectileAbility::OnProjectileReturned(ASingleDamageProjectile* const& Projectile)
{
	Projectile->DisableProjectile();
	ProjectilePool.ReturnObject(Projectile);
}
