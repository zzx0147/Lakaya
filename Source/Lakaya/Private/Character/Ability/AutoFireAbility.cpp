// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AutoFireAbility.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void UAutoFireAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UAutoFireAbility, bIsFiring);
}

UAutoFireAbility::UAutoFireAbility()
{
	InitDelay = FireDelay = 0.2f;
	FireRange = 5000.f;
	FireDamage = 20.f;
}

void UAutoFireAbility::AbilityStart()
{
	if (GetOwner()->HasAuthority())
	{
		Super::AbilityStart();
		return;
	}

	if (bIsFireRequested) return;
	bIsFireRequested = true;
	Super::AbilityStart();
}

void UAutoFireAbility::AbilityStop()
{
	if (GetOwner()->HasAuthority())
	{
		Super::AbilityStop();
		return;
	}

	if (!bIsFireRequested) return;
	bIsFireRequested = false;
	Super::AbilityStop();
}

void UAutoFireAbility::BeginPlay()
{
	Super::BeginPlay();
	if (const auto Character = GetOwner(); Character && Character->HasAuthority())
	{
		Camera = Cast<UCameraComponent>(Character->FindComponentByClass(UCameraComponent::StaticClass()));
		if (!Camera.IsValid())
			UE_LOG(LogInit, Error, TEXT("Fail to find CameraComponent in UAutoFireComponent!"));

		RootComponent = Character->GetRootComponent();
		if (!RootComponent.IsValid()) UE_LOG(LogInit, Error, TEXT("Fail to find RootComponent in UAutoFireComponent!"));

		CollisionQueryParams.AddIgnoredActor(Character);
	}
}

void UAutoFireAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);
	if (bIsFiring) return;
	bIsFiring = true;
	if (auto& TimerManager = GetWorld()->GetTimerManager(); !TimerManager.TimerExists(FireTimer))
	{
		TimerManager.SetTimer(FireTimer, this, &UAutoFireAbility::FireTick, FireDelay, true, InitDelay);
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireTimerSetted!"));
	}
	OnFiringStateChanged.Broadcast(bIsFiring);
}

void UAutoFireAbility::RequestStop_Implementation(const float& RequestTime)
{
	Super::RequestStop_Implementation(RequestTime);
	if (!bIsFiring) return;
	bIsFiring = false;
	OnFiringStateChanged.Broadcast(bIsFiring);
}

void UAutoFireAbility::OnRep_IsFiring()
{
	OnFiringStateChanged.Broadcast(bIsFiring);
}

bool UAutoFireAbility::ShouldFire()
{
	return true;
}

void UAutoFireAbility::SingleFire()
{
	if (!Camera.IsValid() || !RootComponent.IsValid()) return;

	const auto Forward = Camera->GetForwardVector();
	const auto Location = Camera->GetComponentLocation();

	// 캐릭터를 기준으로 정의된 사정거리를 카메라 기준으로 변환하는 수식입니다.
	// (카메라의 전방 벡터에 카메라->캐릭터 벡터를 투영한 길이 + 사정거리) * 카메라 전방 벡터 + 카메라의 위치.
	const auto Destination = Location +
		(Forward.Dot(Location - RootComponent->GetComponentLocation()) + FireRange) * Forward;
	// DrawDebugLine(GetWorld(), Location, Destination, FColor::Red, false, 1.f);

	if (FHitResult Result;
		GetWorld()->LineTraceSingleByChannel(Result, Location, Destination, ECC_Camera, CollisionQueryParams))
	{
		auto Pawn = GetOwner<APawn>();
		UGameplayStatics::ApplyPointDamage(Result.GetActor(), FireDamage, Location, Result,
		                                   Pawn ? Pawn->GetController() : nullptr, GetOwner(), nullptr);
	}
}

void UAutoFireAbility::FailToFire()
{
	bIsFiring = false;
	OnFiringStateChanged.Broadcast(bIsFiring);
}

void UAutoFireAbility::FireTick()
{
	if (!bIsFiring)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimer);
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireTimerClear!"));
	}
	else if (ShouldFire()) SingleFire();
	else FailToFire();
}
