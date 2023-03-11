// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFire.h"

#include "GunComponent.h"
#include "ThirdPersonCharacter.h"
#include "WeaponFireData.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

URiffleFire::URiffleFire()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/WeaponFireDataTable'"));

	if (TableFinder.Succeeded()) WeaponFireDataTable = TableFinder.Object;
}

void URiffleFire::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URiffleFire, BaseDamage, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFire, FireDelay, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFire, FireRange, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFire, SqrFireRange, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFire, SwitchingDelay, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFire, GunComponent, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFire, Character, COND_InitialOnly);
}


void URiffleFire::OnFireStart()
{
	Super::OnFireStart();
	auto& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(SelectorTimer))
		return;

	if (GunComponent.IsValid() && GunComponent->GetRemainBullets() == 0)
	{
		EmptyMagazine();
		return;
	}

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

	if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(false);

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

	GetWorld()->GetTimerManager().SetTimer(SelectorTimer, this, &URiffleFire::UpdateFireMode,
	                                       SwitchingDelay - LockstepDelay);
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

void URiffleFire::SetupData(const FName& RowName)
{
	Super::SetupData(RowName);

	auto Component = Cast<UActorComponent>(GetOuter());
	if (Component == nullptr)
	{
		UE_LOG(LogNetSubObject, Error, TEXT("Outer was not UActorComponent!"));
		return;
	}
	GunComponent = Cast<UGunComponent>(Component);

	Character = Cast<AThirdPersonCharacter>(Component->GetOwner());
	if (!Character.IsValid())
	{
		UE_LOG(LogNetSubObject, Error, TEXT("Attached Actor was not AThirdPersonCharacter."));
		return;
	}

	TraceQueryParams.AddIgnoredActor(Character.Get());

	auto Data = WeaponFireDataTable->FindRow<FWeaponFireData>(RowName,TEXT("RiffleFire"));
	BaseDamage = Data->BaseDamage;
	FireDelay = 60 / Data->FireRate;
	FireRange = Data->FireRange;
	SqrFireRange = FMath::Square(FireRange);
	SwitchingDelay = Data->SelectorSwitchingDelay;
}

void URiffleFire::EmptyMagazine_Implementation()
{
	if (GunComponent.IsValid()) GunComponent->ReloadStart();
}

void URiffleFire::TraceFire()
{
	if (FireCount == 0)
	{
		StopFire();
		return;
	}
	--FireCount;

	if (GunComponent.IsValid() && !GunComponent->CostBullets(1))
	{
		EmptyMagazine();
		StopFire();
		return;
	}

	FHitResult HitResult;
	auto CameraLocation = Character->GetCamera()->GetComponentLocation();
	auto Distance = Character->GetSpringArm()->TargetArmLength;

	// FireRange는 캐릭터를 기준으로 정의된 값이므로 캐릭터와 카메라의 거리를 더해줍니다.
	auto Destination = CameraLocation + Character->GetCamera()->GetForwardVector() * (FireRange + Distance);

	// Trace from camera
	DrawDebugLine(GetWorld(), CameraLocation, Destination, FColor::Red, false, 1);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, Destination, ECC_Camera, TraceQueryParams))
		return;

	// 캐릭터를 기준으로 FireRange보다 밖에 있는 경우 배제합니다.
	if (SqrFireRange < (HitResult.ImpactPoint - Character->GetActorLocation()).SquaredLength()) return;

	UGameplayStatics::ApplyDamage(HitResult.GetActor(), BaseDamage, Character->GetController(),
	                              Character.Get(), nullptr);
}

void URiffleFire::StopFire()
{
	FireCount = 0;
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void URiffleFire::UpdateFireMode()
{
	Selector = DesiredSelector;
	if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(true);
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

	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &URiffleFire::TraceFire, FireDelay, true, 0.f);
}
