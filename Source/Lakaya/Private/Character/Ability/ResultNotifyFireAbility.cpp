// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ResultNotifyFireAbility.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/BulletComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

UResultNotifyFireAbility::UResultNotifyFireAbility()
{
	bWantsInitializeComponent = true;
	FireDelay = 0.2f;
	FireRange = 10000.f;
	FireDamage = 20.f;
	bShouldFireSmoothing = false;
	PoolCount = 20;
	DecalShowingTime = 10.f;
	BulletCost = 1;
}

void UResultNotifyFireAbility::AbilityStart()
{
	if (bWantsToFire) return;
	if (!GetOwner()->HasAuthority()) bWantsToFire = true;
	Super::AbilityStart();
}

void UResultNotifyFireAbility::AbilityStop()
{
	if (!bWantsToFire) return;
	if (!GetOwner()->HasAuthority()) bWantsToFire = false;
	Super::AbilityStop();
}

void UResultNotifyFireAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (!AliveState && GetOwner()->HasAuthority())
	{
		bWantsToFire = false;
		ClearFireTimer();
	}
}

void UResultNotifyFireAbility::InitializeComponent()
{
	Super::InitializeComponent();
	BulletComponent = GetOwner()->FindComponentByClass<UBulletComponent>();
	if (auto MuzzleComponents = GetOwner()->GetComponentsByTag(UArrowComponent::StaticClass(), FName("Muzzle"));
		MuzzleComponents.IsValidIndex(0))
		MuzzleComponent = Cast<UArrowComponent>(MuzzleComponents[0]);
	CollisionQueryParams.AddIgnoredActor(GetOwner());

	// 데칼 오브젝트 풀 생성
	for (const auto& Pair : DecalClasses)
	{
		if (!Pair.Value) continue;
		DecalPool.Emplace(Pair.Key).SetupObjectPool(PoolCount, [this, DecalClass = Pair.Value]
		{
			const auto Actor = GetWorld()->SpawnActor<AActor>(DecalClass);
			Actor->SetActorLocation(FVector(-9999.f, -9999.f, -9999.f));
			return Actor;
		});
	}
}

void UResultNotifyFireAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);
	if (bWantsToFire || !GetAliveState()) return;
	bWantsToFire = true;
	if (auto& TimerManager = GetWorld()->GetTimerManager(); !TimerManager.TimerExists(FireTimer))
	{
		TimerManager.SetTimer(FireTimer, this, &UResultNotifyFireAbility::FireTick, FireDelay, true, FirstFireDelay);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireTimerSetted!"));
	}
}

void UResultNotifyFireAbility::RequestStop_Implementation(const float& RequestTime)
{
	Super::RequestStop_Implementation(RequestTime);
	if (!bWantsToFire || !GetAliveState()) return;
	bWantsToFire = false;
}

void UResultNotifyFireAbility::FireTick()
{
	if (!bWantsToFire) ClearFireTimer();
	else if (ShouldFire()) SingleFire();
	else FailToFire();
}

bool UResultNotifyFireAbility::ShouldFire()
{
	// BulletComponent가 존재하는 경우 CostBullet에 실패하면 false를 반환합니다. 
	return !BulletComponent.IsValid() || BulletComponent->CostBullet(BulletCost);
}

void UResultNotifyFireAbility::SingleFire()
{
	const auto LineStart = MuzzleComponent.IsValid()
		                       ? MuzzleComponent->GetComponentLocation()
		                       : GetOwner()->GetActorLocation();
	auto End = GetCameraForwardTracePoint(FireRange, CollisionQueryParams);

	FHitResult Result;
	if (GetWorld()->LineTraceSingleByChannel(Result, LineStart, End, ECC_Visibility, CollisionQueryParams))
	{
		End = Result.ImpactPoint;
		const auto Pawn = GetOwner<APawn>();
		UGameplayStatics::ApplyPointDamage(Result.GetActor(), FireDamage, LineStart, Result,
		                                   Pawn ? Pawn->GetController() : nullptr, GetOwner(), nullptr);
	}
	InvokeFireNotify(Result);
	// DrawDebugLine(GetWorld(), LineStart, End, FColor::Red, false, 2.f);
}

void UResultNotifyFireAbility::FailToFire()
{
	bWantsToFire = false;
	ClearFireTimer();
}

void UResultNotifyFireAbility::ClearFireTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireTimerClear!"));
}

void UResultNotifyFireAbility::InvokeFireNotify(const FHitResult& HitResult)
{
	const auto End = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd;
	const auto Normal = HitResult.bBlockingHit ? HitResult.ImpactNormal : FVector::ZeroVector;
	const auto Actor = HitResult.GetActor();
	const auto FireResult = Actor
		                        ? Actor->IsA<ALakayaBaseCharacter>()
			                          ? EFireResult::Creature
			                          : EFireResult::Environment
		                        : EFireResult::None;

	if (bShouldFireSmoothing) NotifyFireResult(End, Normal, FireResult);
	else NotifySingleFire(HitResult.TraceStart, End, Normal, FireResult);
}

void UResultNotifyFireAbility::DrawDecal(const FVector& Location, const FVector& Normal, const EFireResult& Kind)
{
	if (!DecalPool.Contains(Kind)) return;
	if (const auto Decal = DecalPool[Kind].GetObject())
	{
		Decal->SetActorLocationAndRotation(Location, Normal.Rotation());

		// 일정시간 뒤 보이지 않는 곳으로 이동시켜두고, 다시 오브젝트 풀에 밀어넣습니다.
		FTimerHandle TempTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TempTimerHandle, [this, Decal, Kind]
		{
			Decal->SetActorLocation(FVector(-9999.f, -9999.f, -9999.f));
			if (DecalPool.Contains(Kind)) DecalPool[Kind].ReturnObject(Decal);
			else Decal->Destroy();
		}, DecalShowingTime, false);
	}
}

void UResultNotifyFireAbility::DrawTrail(const FVector& Start, const FVector& End)
{
	if (!TrailNiagaraSystem) return;
	if (const auto Niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TrailNiagaraSystem, Start))
		Niagara->SetNiagaraVariableVec3(TEXT("BeamEnd"), End - Start);

	// TODO : GunImpact
	if (!GunImpactSystem) return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GunImpactSystem, Start);
}

void UResultNotifyFireAbility::DrawImpact(const FVector& Location, const FVector& Normal, const EFireResult& Kind)
{
	if (!ImpactNiagaraSystems.Contains(Kind)) return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactNiagaraSystems[Kind], Location, Normal.Rotation());
}

void UResultNotifyFireAbility::NotifySingleFire_Implementation(const FVector& Start, const FVector& End,
                                                               const FVector& Normal, const EFireResult& FireResult)
{
	DrawTrail(Start, End);
	DrawDecal(End, Normal, FireResult);
	DrawImpact(End, Normal, FireResult);
	// DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f);
}

void UResultNotifyFireAbility::NotifyFireResult_Implementation(const FVector& HitPoint, const FVector& Normal,
                                                               const EFireResult& FireResult)
{
	const FVector Start = MuzzleComponent.IsValid()
		                      ? MuzzleComponent->GetComponentLocation()
		                      : GetOwner()->GetActorLocation();
	DrawTrail(Start, HitPoint);
	DrawDecal(HitPoint, Normal, FireResult);
	DrawImpact(HitPoint, Normal, FireResult);
	DrawDebugLine(GetWorld(), Start, HitPoint, FColor::Green, false, 2.f);
}
