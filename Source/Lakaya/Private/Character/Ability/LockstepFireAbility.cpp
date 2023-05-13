// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LockstepFireAbility.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"


ULockstepFireAbility::ULockstepFireAbility()
{
	bWantsInitializeComponent = true;
	LockstepDelay = 0.1f;
	FirstFireDelay = 0.f;
	FireDelay = 0.2f;
	BaseDamage = 20.f;
	FireRange = 5000.f;
}

void ULockstepFireAbility::InitializeComponent()
{
	Super::InitializeComponent();
	CollisionQueryParams.AddIgnoredActor(GetOwner());

	Camera = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (!Camera.IsValid()) UE_LOG(LogInit, Error, TEXT("Camera was nullptr on ULockstepFireAbility::OnRegister"));
}

void ULockstepFireAbility::AbilityStart()
{
	if (!ShouldAbilityStart()) return;
	if (!GetOwner()->HasAuthority()) SetWantsToFire(true);
	Super::AbilityStart();
}

void ULockstepFireAbility::AbilityStop()
{
	if (!bWantsToFire) return;
	if (!GetOwner()->HasAuthority()) SetWantsToFire(false);
	Super::AbilityStop();
}

void ULockstepFireAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);
	if (bWantsToFire) return;
	SetWantsToFire(true);

	// 타이머가 없다면 설정합니다.
	if (auto& TimerManager = GetWorld()->GetTimerManager(); !TimerManager.TimerExists(FireTimer))
	{
		TimerManager.SetTimer(FireTimer, this, &ULockstepFireAbility::FireTick, FireDelay, true, FirstFireDelay);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireTimerSetted!"));
	}
}

void ULockstepFireAbility::RequestStop_Implementation(const float& RequestTime)
{
	Super::RequestStop_Implementation(RequestTime);
	if (bWantsToFire) SetWantsToFire(false);
}

void ULockstepFireAbility::FireTick()
{
	if (!bWantsToFire) ClearFireTimer();
	else if (ShouldFire()) InvokeFireEvent();
	else FailToFire();
}

bool ULockstepFireAbility::ShouldFire()
{
	return true;
}

void ULockstepFireAbility::SingleFire(const FLockstepFireInfo& FireInfo)
{
	// 카메라로부터 CameraDeadZone만큼 떨어진 곳에서부터 충돌판정을 시작하도록 합니다.
	const auto Start = FireInfo.GetCameraForward(CameraDeadZone);

	// 캐릭터를 중심으로 정의된 FireRange까지만 충돌판정이 이뤄지도록 합니다.
	auto End = FireInfo.GetProjectedForward(FireRange);

	// Start와 End 사이에 부딫치는 물체가 존재한다면, 부딫친 위치로 End를 업데이트합니다.
	if (FHitResult Result;
		GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_Visibility, CollisionQueryParams))
		End = Result.ImpactPoint;
	DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 2.f);

	// 액터로부터 End까지 트레이스하여 실질적인 사격 판정을 수행합니다.
	if (FHitResult Result;
		GetWorld()->LineTraceSingleByChannel(Result, FireInfo.ActorLocation, End, ECC_Visibility, CollisionQueryParams))
	{
		End = Result.ImpactPoint;
		const auto Owner = GetOwner<APawn>();
		if (GetOwner()->HasAuthority())
			UGameplayStatics::ApplyPointDamage(Result.GetActor(), BaseDamage, FireInfo.ActorLocation, Result,
			                                   Owner ? Owner->GetController() : nullptr, GetOwner(), nullptr);
	}
	DrawDebugLine(GetWorld(), FireInfo.ActorLocation, End, FColor::Red, false, 2.f);
}

void ULockstepFireAbility::FailToFire()
{
	SetWantsToFire(false);
	ClearFireTimer();
}

void ULockstepFireAbility::SetWantsToFire(const bool& FireState)
{
	bWantsToFire = FireState;
	OnWantsToFireChanged.Broadcast(bWantsToFire);
}

void ULockstepFireAbility::ClearFireTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireTimerClear!"));
}

bool ULockstepFireAbility::ShouldAbilityStart()
{
	return !bWantsToFire;
}

void ULockstepFireAbility::LockstepCallback()
{
	const float CurrentTime = GetServerTime();
	FLockstepFireInfo FireInfo;
	do
	{
		FireInfos.HeapPop(FireInfo, false);
		SingleFire(FireInfo);
	}
	while (!FireInfos.IsEmpty() && FireInfos.HeapTop().Time <= CurrentTime);
	UpdateLockstepTimer(CurrentTime);
}

void ULockstepFireAbility::InvokeFireEvent()
{
	if (const auto LocalOwner = GetOwner(); LocalOwner && Camera.IsValid())
		NotifyFire(FLockstepFireInfo
			{
				GetServerTime() + LockstepDelay,
				Camera->GetComponentLocation(),
				Camera->GetForwardVector(),
				LocalOwner->GetActorLocation()
			});
}

void ULockstepFireAbility::UpdateLockstepTimer(const float& CurrentTime)
{
	if (FireInfos.IsEmpty())
	{
		RecentTopFireTime = FLT_MAX;
		return;
	}

	// 힙의 맨 윗 이벤트가 변경되었다면 타이머를 새로 셋팅합니다.
	if (const auto TopTime = FireInfos.HeapTop().Time; RecentTopFireTime != TopTime)
	{
		GetWorld()->GetTimerManager().SetTimer(LockstepTimer, this, &ULockstepFireAbility::LockstepCallback,
		                                       TopTime - CurrentTime);
		RecentTopFireTime = TopTime;
	}
}

void ULockstepFireAbility::NotifyFire_Implementation(const FLockstepFireInfo& FireInfo)
{
	const auto CurrentTime = GetServerTime();

	// 유효하지 않은 사격 신호라면 스킵합니다.
	if (FireInfo.Time < CurrentTime) return;

	FireInfos.HeapPush(FireInfo);
	UpdateLockstepTimer(CurrentTime);
}
