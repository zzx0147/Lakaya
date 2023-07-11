// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ResultNotifyFireAbility.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/LakayaBaseCharacter.h"
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
	bCanEverStopRemoteCall = bCanEverStartRemoteCall = true;
	CollapsedLocation = FVector(-9999.f,-9999.f,-9999.f);
}

bool UResultNotifyFireAbility::ShouldStartRemoteCall()
{
	if (bWantsToFire) return false;
	if (!GetOwner()->HasAuthority()) SetWantsToFire(true);
	return true;
}

bool UResultNotifyFireAbility::ShouldStopRemoteCall()
{
	if (!bWantsToFire) return false;
	if (!GetOwner()->HasAuthority()) SetWantsToFire(false);
	return true;
}

void UResultNotifyFireAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (!AliveState && GetOwner()->HasAuthority())
	{
		SetWantsToFire(false);
		ClearFireTimer();
	}
}

void UResultNotifyFireAbility::InitializeComponent()
{
	Super::InitializeComponent();
	CollisionQueryParams.AddIgnoredActor(GetOwner());

	// 총구 이펙트 생성
	if (GunImpactSystem && BasisComponent)
	{
		GunImpactNiagara =
			UNiagaraFunctionLibrary::SpawnSystemAttached(GunImpactSystem, BasisComponent.Get(), NAME_None,
			                                             FVector::ZeroVector, FRotator::ZeroRotator,
			                                             EAttachLocation::SnapToTarget, false, false);
	}

	// 데칼 오브젝트 풀 생성
	for (const auto& Pair : DecalClasses)
	{
		if (!Pair.Value) continue;
		DecalPool.Emplace(Pair.Key).SetupObjectPool(PoolCount, [this, DecalClass = Pair.Value]
		{
			const auto Actor = GetWorld()->SpawnActor<AActor>(DecalClass);
			if (Actor) Actor->SetActorLocation(CollapsedLocation);
			return Actor;
		});
	}
}

void UResultNotifyFireAbility::RemoteAbilityStart(const float& RequestTime)
{
	Super::RemoteAbilityStart(RequestTime);
	if (bWantsToFire || !GetAliveState()) return;
	SetWantsToFire(true);

	if (auto& TimerManager = GetWorld()->GetTimerManager(); !TimerManager.TimerExists(FireTimer))
	{
		TimerManager.SetTimer(FireTimer, this, &UResultNotifyFireAbility::FireTick, FireDelay, true, FirstFireDelay);
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireTimerSetted!"));
	}
}

void UResultNotifyFireAbility::RemoteAbilityStop(const float& RequestTime)
{
	Super::RemoteAbilityStop(RequestTime);
	if (!bWantsToFire || !GetAliveState()) return;
	SetWantsToFire(false);
}

void UResultNotifyFireAbility::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	if (const auto World = GetWorld()) World->GetTimerManager().ClearAllTimersForObject(this);
}

void UResultNotifyFireAbility::SetBasisComponent(USceneComponent* NewComponent)
{
	BasisComponent = NewComponent;
}

void UResultNotifyFireAbility::FireTick()
{
	if (!bWantsToFire) ClearFireTimer();
	else if (ShouldFire()) SingleFire();
	else FailToFire();
}

bool UResultNotifyFireAbility::ShouldFire()
{
	return CostResource(FireCost);
}

void UResultNotifyFireAbility::SingleFire()
{
	const auto LineStart = BasisComponent ? BasisComponent->GetComponentLocation() : GetOwner()->GetActorLocation();
	auto End = GetCameraForwardTracePoint(FireRange, CollisionQueryParams);

	FHitResult Result;
	if (GetWorld()->LineTraceSingleByChannel(Result, LineStart, End, ECC_Visibility, CollisionQueryParams))
	{
		End = Result.ImpactPoint;
		const auto Pawn = GetOwner<APawn>();
		UGameplayStatics::ApplyPointDamage(Result.GetActor(), GetTerminalDamage(Result), LineStart, Result,
		                                   Pawn ? Pawn->GetController() : nullptr, GetOwner(), nullptr);
	}
	InvokeFireNotify(Result);
	// DrawDebugLine(GetWorld(), LineStart, End, FColor::Red, false, 2.f);
}

void UResultNotifyFireAbility::FailToFire()
{
	SetWantsToFire(false);
	ClearFireTimer();
}

void UResultNotifyFireAbility::ClearFireTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireTimerClear!"));
}

float UResultNotifyFireAbility::GetTerminalDamage(const FHitResult& HitResult)
{
	return FireDamage * (WeakPointMultiplier.Contains(HitResult.BoneName)
		                     ? WeakPointMultiplier[HitResult.BoneName]
		                     : 1.f);
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
		static FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UResultNotifyFireAbility::DisableDecal, Decal, Kind);
		GetWorld()->GetTimerManager().SetTimer(TempTimerHandle, TimerDelegate, DecalShowingTime, false);
	}
}

void UResultNotifyFireAbility::DrawTrail(const FVector& Start, const FVector& End)
{
	if (!TrailNiagaraSystem) return;
	if (const auto Niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TrailNiagaraSystem, Start))
		Niagara->SetNiagaraVariableVec3(TEXT("BeamEnd"), End - Start);
}

void UResultNotifyFireAbility::DrawImpact(const FVector& Location, const FVector& Normal, const EFireResult& Kind)
{
	if (!ImpactNiagaraSystems.Contains(Kind)) return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactNiagaraSystems[Kind], Location, Normal.Rotation());
}

void UResultNotifyFireAbility::SetWantsToFire(const bool& FireState)
{
	bWantsToFire = FireState;
	OnWantsToFireChanged.Broadcast(bWantsToFire);
}

void UResultNotifyFireAbility::DisableDecal(AActor* Decal, EFireResult Kind)
{
	if (!Decal) return;
	Decal->SetActorLocation(CollapsedLocation);
	if (DecalPool.Contains(Kind)) DecalPool[Kind].ReturnObject(Decal);
	else Decal->Destroy();
}

void UResultNotifyFireAbility::NotifySingleFire_Implementation(const FVector& Start, const FVector& End,
                                                               const FVector& Normal, const EFireResult& FireResult)
{
	DrawTrail(Start, End);
	DrawDecal(End, Normal, FireResult);
	DrawImpact(End, Normal, FireResult);
	if (GunImpactNiagara.IsValid()) GunImpactNiagara->Activate(true);
	OnSingleFire.Broadcast(Start, End, Normal, FireResult);
	// DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f);
}

void UResultNotifyFireAbility::NotifyFireResult_Implementation(const FVector& HitPoint, const FVector& Normal,
                                                               const EFireResult& FireResult)
{
	const FVector Start = BasisComponent ? BasisComponent->GetComponentLocation() : GetOwner()->GetActorLocation();
	DrawTrail(Start, HitPoint);
	DrawDecal(HitPoint, Normal, FireResult);
	DrawImpact(HitPoint, Normal, FireResult);
	if (GunImpactNiagara.IsValid()) GunImpactNiagara->Activate(true);
	OnSingleFire.Broadcast(Start, HitPoint, Normal, FireResult);
	//DrawDebugLine(GetWorld(), Start, HitPoint, FColor::Green, false, 2.f);
}
