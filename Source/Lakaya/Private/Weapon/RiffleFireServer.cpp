// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RiffleFireServer.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Weapon/GunComponent.h"
#include "Weapon/WeaponFireData.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

URiffleFireServer::URiffleFireServer()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/DT_WeaponFireDataTable'"));

	static const ConstructorHelpers::FClassFinder<AActor> DecalActorFinder(
		TEXT("/Game/Blueprints/ETC/BP_GunDecal.BP_GunDecal_C"));

	if (TableFinder.Succeeded()) WeaponFireDataTable = TableFinder.Object;
	DecalActorClass = DecalActorFinder.Class;
}

void URiffleFireServer::OnFireStart()
{
	Super::OnFireStart();
	FireStartCore(FireTimer, EFocusContext::Server, FireCount, [this]
	{
		SetFireCount(Selector, FireCount);
	}, [this]
	{
		FreshFireCore(Selector, FireCount, FireTimer, [this]
		{
			FireCallback(FireCount, FireTimer, EFocusContext::Server, [this]
			{
				return !GunComponent->CostBullets(1);
			}, [this]
			{
				if (!Character->ReleaseFocus(EFocusContext::Server, EFocusSpace::MainHand, EFocusState::Firing))
					UE_LOG(LogNetSubObject, Error, TEXT("Fail to release focus on OnFireStart with authority!"));
			}, [this] { TraceFire(); });
		});
	});
}

void URiffleFireServer::OnFireStop()
{
	Super::OnFireStop();
	FireStopCore(Selector, FireCount, EFocusContext::Server);
}

void URiffleFireServer::OnCharacterDead()
{
	Super::OnCharacterDead();
	OnFireStop();
}

void URiffleFireServer::TraceFire_Implementation()
{
	FHitResult HitResult;
	auto CameraLocation = Character->GetCamera()->GetComponentLocation();
	auto Distance = Character->GetSpringArm()->TargetArmLength;

	// FireRange는 캐릭터를 기준으로 정의된 값이므로 캐릭터와 카메라의 거리를 더해줍니다.
	auto Destination = CameraLocation + Character->GetCamera()->GetForwardVector() * (FireRange + Distance);

	// Trace from camera
	// DrawDebugLine(GetWorld(), CameraLocation, Destination, FColor::Red, false, 1);
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, Destination, ECC_Camera, TraceQueryParams))
		return;

	// 캐릭터를 기준으로 FireRange보다 밖에 있는 경우 배제합니다.
	if (SqrFireRange < (HitResult.ImpactPoint - Character->GetActorLocation()).SquaredLength()) return;

	UGameplayStatics::ApplyDamage(HitResult.GetActor(), BaseDamage, Character->GetController(),
								  Character.Get(), nullptr);
	if (Cast<APawn>(HitResult.GetActor()) == nullptr)
	{
		//FActorSpawnParameters SpawnParam;		
		GetWorld()->SpawnActor<AActor>(DecalActorClass, HitResult.Location, HitResult.Normal.Rotation());
		
		FSoftObjectPath DecalEffectPath;
		DecalEffectPath = TEXT("/Game/Effects/M_VFX/VFX_Decalimpact");
		UNiagaraSystem* DecalEffectNiagara = Cast<UNiagaraSystem>(DecalEffectPath.TryLoad());
		UNiagaraComponent* NiagaraComponent;
		if (DecalEffectNiagara)
		{
			NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DecalEffectNiagara,
				HitResult.Location, HitResult.Normal.Rotation());
		}
	}
}

void URiffleFireServer::SetupData(const FName& RowName)
{
	Super::SetupData(RowName);

	auto Component = Cast<UActorComponent>(GetOuter());
	if (Component == nullptr)
	{
		UE_LOG(LogNetSubObject, Fatal, TEXT("Outer was not UActorComponent!"));
		return;
	}
	GunComponent = Cast<UGunComponent>(Component);

	Character = Cast<AFocusableCharacter>(Component->GetOwner());
	if (!Character.IsValid())
	{
		UE_LOG(LogNetSubObject, Fatal, TEXT("Attached Actor was not AThirdPersonCharacter."));
		return;
	}

	TraceQueryParams.AddIgnoredActor(Character.Get());

	auto Data = WeaponFireDataTable->FindRow<FWeaponFireData>(RowName,TEXT("RiffleFire"));
	BaseDamage = Data->BaseDamage;
	OriginBaseDamage = BaseDamage;
	FireDelay = 60 / Data->FireRate;
	FireRange = Data->FireRange;
	SqrFireRange = FMath::Square(FireRange);
}

void URiffleFireServer::EmptyMagazine_Implementation()
{
	GunComponent->ReloadStart();
}