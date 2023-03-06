// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFire.h"

#include "ThirdPersonCharacter.h"
#include "WeaponFireData.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"

URiffleFire::URiffleFire()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/WeaponFireDataTable'"));

	if (TableFinder.Succeeded()) WeaponFireDataTable = TableFinder.Object;
}

void URiffleFire::OnFireStart()
{
	Super::OnFireStart();
	auto& TimerManager = GetWorld()->GetGameState()->GetWorldTimerManager();
	if (TimerManager.IsTimerActive(SelectorTimer)) return;
	if (TimerManager.IsTimerActive(FireTimer)) OnNestedFire();
	else OnFreshFire();
}

void URiffleFire::OnFireStop()
{
	Super::OnFireStop();
	if (Selector == EGunSelector::Auto) FireCount = 0;
}

void URiffleFire::OnSwitchSelector()
{
	Super::OnSwitchSelector();
	if (IsOnFiring()) return;

	switch (DesiredSelector)
	{
	case EGunSelector::Semi: DesiredSelector = EGunSelector::Burst;
		break;
	case EGunSelector::Burst: DesiredSelector = EGunSelector::Auto;
		break;
	case EGunSelector::Auto: DesiredSelector = EGunSelector::Semi;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("DesiredFire was not EFireMode"));
		return;
	}

	GetWorld()->GetGameState()->GetWorldTimerManager().SetTimer(SelectorTimer, this, &URiffleFire::UpdateFireMode,
	                                                            FireDelay);
}

void URiffleFire::OnFireStartNotify()
{
	Super::OnFireStartNotify();
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireStart"));
}

void URiffleFire::OnFireStopNotify()
{
	Super::OnFireStopNotify();
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("FireStop"));
}

void URiffleFire::OnSwitchSelectorNotify()
{
	Super::OnSwitchSelectorNotify();
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Switch"));
}

void URiffleFire::SetupData_Implementation(const FName& RowName)
{
	Super::SetupData_Implementation(RowName);
	auto Component = Cast<UActorComponent>(GetOuter());
	if (Component == nullptr)
	{
		UE_LOG(LogNetSubObject, Error, TEXT("Outer was not UActorComponent!"));
		return;
	}

	Character = Cast<AThirdPersonCharacter>(Component->GetOwner());
	if (Character.IsStale())
	{
		UE_LOG(LogNetSubObject, Error, TEXT("Attached Actor was not AThirdPersonCharacter."));
		return;
	}

	TraceQueryParams.AddIgnoredActor(Character.Get());

	auto Data = WeaponFireDataTable->FindRow<FWeaponFireData>(RowName,TEXT("RiffleFire"));
	BaseDamage = Data->BaseDamage;
	FireDelay = 60 / Data->FireRate;
	FireRange = Data->FireRange;
	SwitchingDelay = Data->SelectorSwitchingDelay;
}

void URiffleFire::TraceFire()
{
	if (FireCount == 0)
	{
		StopFire();
		return;
	}
	--FireCount;

	//TODO: 사거리를 제한하는 로직을 추가합니다.
	FHitResult HitResult;
	const auto CameraLocation = Character->GetCamera()->GetComponentLocation();
	const auto CameraForward = CameraLocation + Character->GetCamera()->GetForwardVector() * 10000;
	FVector AimPoint;

	// Get camera watching point
	DrawDebugLine(GetWorld(), CameraLocation, CameraForward, FColor::Green, false, 1);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, CameraForward, ECC_Camera, TraceQueryParams))
		AimPoint = HitResult.ImpactPoint;
	else AimPoint = CameraForward;

	// Trace from actor to camera watching point
	const auto& Location = Character->GetActorLocation();
	DrawDebugLine(GetWorld(), Location, AimPoint, FColor::Red, false, 1);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Location, AimPoint, ECC_GameTraceChannel2, TraceQueryParams))
		return;

	UGameplayStatics::ApplyDamage(HitResult.GetActor(), BaseDamage, Character->GetController(),
	                              Character.Get(), nullptr);
}

void URiffleFire::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void URiffleFire::UpdateFireMode()
{
	Selector = DesiredSelector;
}

void URiffleFire::OnNestedFire()
{
	switch (Selector)
	{
	case EGunSelector::Semi:
		if (IsNotFiring()) FireCount = 1;
		break;
	case EGunSelector::Burst:
		if (IsNotFiring()) FireCount = 3;
		break;
	case EGunSelector::Auto:
		FireCount = MAX_uint16;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("FireMode was not EFireMode"));
		return;
	}
}

void URiffleFire::OnFreshFire()
{
	switch (Selector)
	{
	case EGunSelector::Semi: FireCount = 1;
		break;
	case EGunSelector::Burst: FireCount = 3;
		break;
	case EGunSelector::Auto: FireCount = MAX_uint16;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("FireMode was not EFireMode"));
		return;
	}

	GetWorld()->GetGameState()->GetWorldTimerManager().SetTimer(FireTimer, this, &URiffleFire::TraceFire, FireDelay,
	                                                            true, 0.f);
}
