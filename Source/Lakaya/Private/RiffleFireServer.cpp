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

void URiffleFireServer::OnFireStart()
{
	Super::OnFireStart();
	FireStartCore(SelectorTimer, FireTimer,
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
	SwitchSelectorCore(FireCount, DesiredSelector, SelectorTimer,
	                   [this] { UpdateSelector(DesiredSelector, Selector); });
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
