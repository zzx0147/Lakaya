// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ResultNotifyFireAbility.h"

#include "Camera/CameraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

UResultNotifyFireAbility::UResultNotifyFireAbility()
{
	bWantsInitializeComponent = true;
	FireDelay = 0.2f;
	FireRange = 10000.f;
	FireDamage = 20.f;
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

void UResultNotifyFireAbility::InitializeComponent()
{
	Super::InitializeComponent();
	Camera = GetOwner()->FindComponentByClass<UCameraComponent>();
	CollisionQueryParams.AddIgnoredActor(GetOwner());
}

void UResultNotifyFireAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);
	if (bWantsToFire) return;
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
	if (!bWantsToFire) return;
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
	return true;
}

void UResultNotifyFireAbility::SingleFire()
{
	if (!Camera.IsValid() || !GetOwner()) return;
	const auto CameraForward = Camera->GetForwardVector();
	const auto CameraLocation = Camera->GetComponentLocation();
	const auto ActorLocation = GetOwner()->GetActorLocation();
	auto End = CameraLocation + CameraForward * (CameraForward.Dot(CameraLocation - ActorLocation) + FireRange);

	FHitResult Result;
	if (GetWorld()->LineTraceSingleByChannel(Result, CameraLocation, End, ECC_Visibility, CollisionQueryParams))
		End = Result.ImpactPoint;
	DrawDebugLine(GetWorld(), CameraLocation, End, FColor::Cyan, false, 2.f);

	if (GetWorld()->LineTraceSingleByChannel(Result, ActorLocation, End, ECC_Visibility, CollisionQueryParams))
	{
		End = Result.ImpactPoint;
		UGameplayStatics::ApplyPointDamage(Result.GetActor(), FireDamage, ActorLocation, Result,
		                                   GetTypedOuter<AController>(), GetOwner(), nullptr);
		NotifyFireResult(End, Result.ImpactNormal,
		                 Result.GetActor()
			                 ? Result.GetActor()->IsA<ALakayaBaseCharacter>()
				                   ? EFireResult::Creature
				                   : EFireResult::Environment
			                 : EFireResult::None);
	}
	else NotifyFireResult(End, FVector::ZeroVector, EFireResult::None);
	DrawDebugLine(GetWorld(), ActorLocation, End, FColor::Red, false, 2.f);
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

void UResultNotifyFireAbility::NotifyFireResult_Implementation(const FVector& HitPoint, const FVector& Normal,
                                                               const EFireResult& FireResult)
{
	DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), HitPoint, FColor::Green, false, 2.f);
}
