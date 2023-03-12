// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFireServer.h"

#include "GunComponent.h"
#include "ThirdPersonCharacter.h"
#include "WeaponFireData.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

URiffleFireServer::URiffleFireServer()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/WeaponFireDataTable'"));

	if (TableFinder.Succeeded()) WeaponFireDataTable = TableFinder.Object;
}

void URiffleFireServer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URiffleFireServer, BaseDamage, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireServer, FireDelay, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireServer, FireRange, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireServer, SqrFireRange, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireServer, SwitchingDelay, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireServer, GunComponent, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireServer, Character, COND_InitialOnly);
}


void URiffleFireServer::OnFireStart()
{
	Super::OnFireStart();
	FireStartCore(SelectorTimer,
	              [this] { return GunComponent.IsValid() && GunComponent->GetRemainBullets() <= 0; },
	              [this] { EmptyMagazine(); },
	              [this] { NestedFireCore(Selector, FireCount); },
	              [this]
	              {
		              FreshFireCore(Selector, FireCount, FireTimer,
		                            [this]
		                            {
			                            FireCallback(FireCount, FireTimer,
			                                         [this]
			                                         {
				                                         return GunComponent.IsValid() && !GunComponent->CostBullets(1);
			                                         },
			                                         [this] { EmptyMagazine(); },
			                                         [this] { TraceFire(); });
		                            });
	              });
}

void URiffleFireServer::OnFireStop()
{
	Super::OnFireStop();
	FireStopCore(Selector, FireCount);
}

void URiffleFireServer::OnSwitchSelector()
{
	Super::OnSwitchSelector();
	SwitchSelectorCore(FireCount, DesiredSelector, SelectorTimer, [this] { UpdateFireMode(); });
}

void URiffleFireServer::FireStartCore(FTimerHandle& GunSelectorTimer, std::function<bool()> EmptyDeterminant,
                                      std::function<void()> OnEmptyMag, std::function<void()> NestedFire,
                                      std::function<void()> FreshFire)
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(GunSelectorTimer))
		return;

	if (EmptyDeterminant())
	{
		if (OnEmptyMag != nullptr) OnEmptyMag();
		return;
	}

	if (TimerManager.IsTimerActive(FireTimer)) NestedFire();
	else FreshFire();
}

void URiffleFireServer::FireStopCore(const EGunSelector& GunSelector, uint16& GunFireCount)
{
	if (GunSelector == EGunSelector::Auto) GunFireCount = 0;
}

void URiffleFireServer::SwitchSelectorCore(const uint16& GunFireCount, EGunSelector& GunSelector,
                                           FTimerHandle& GunSelectorTimer, std::function<void()> OnUpdateSelector)
{
	if (IsOnFiring()) return;

	if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(false);

	switch (GunSelector)
	{
	case EGunSelector::Semi: GunSelector = EGunSelector::Burst;
		break;
	case EGunSelector::Burst: GunSelector = EGunSelector::Auto;
		break;
	case EGunSelector::Auto: GunSelector = EGunSelector::Semi;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("DesiredFire was not EFireMode"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(GunSelectorTimer, OnUpdateSelector, SwitchingDelay - LockstepDelay, false);
}

void URiffleFireServer::NestedFireCore(const EGunSelector& GunSelector, uint16& GunFireCount)
{
	switch (GunSelector)
	{
	case EGunSelector::Semi:
		if (GunFireCount == 0) GunFireCount = 1;
		break;
	case EGunSelector::Burst:
		if (GunFireCount == 0) GunFireCount = 3;
		break;
	case EGunSelector::Auto:
		GunFireCount = MAX_uint16;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("GunSelector was not EFireMode"));
		return;
	}
}

void URiffleFireServer::FreshFireCore(const EGunSelector& GunSelector, uint16& GunFireCount,
                                      FTimerHandle& FireTimerHandle,
                                      std::function<void()> FireFunction)
{
	switch (GunSelector)
	{
	case EGunSelector::Semi: GunFireCount = 1;
		break;
	case EGunSelector::Burst: GunFireCount = 3;
		break;
	case EGunSelector::Auto: GunFireCount = MAX_uint16;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("GunSelector was not EFireMode"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, FireFunction, FireDelay, true, 0.f);
}

void URiffleFireServer::SetupData(const FName& RowName)
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

void URiffleFireServer::EmptyMagazine_Implementation()
{
	if (GunComponent.IsValid()) GunComponent->ReloadStart();
}

void URiffleFireServer::FireCallback(uint16& GunFireCount, FTimerHandle& FireTimerHandle,
                                     std::function<bool()> EmptyDeterminant,
                                     std::function<void()> OnEmpty, std::function<void()> FireFunction)
{
	if (GunFireCount == 0)
	{
		StopFireCore(GunFireCount, FireTimerHandle);
		return;
	}
	--GunFireCount;

	if (EmptyDeterminant())
	{
		if (OnEmpty != nullptr) OnEmpty();
		StopFireCore(GunFireCount, FireTimerHandle);
		return;
	}

	FireFunction();
}

void URiffleFireServer::StopFireCore(uint16& GunFireCount, FTimerHandle& FireTimerHandle)
{
	GunFireCount = 0;
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void URiffleFireServer::TraceFire()
{
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

void URiffleFireServer::UpdateFireMode()
{
	Selector = DesiredSelector;
	if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(true);
}
