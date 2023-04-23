// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RiffleFireClient.h"

#include "Weapon/GunComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"

void URiffleFireClient::OnFireStartNotify()
{
	Super::OnFireStartNotify();

	FireStartCore(FireTimer, EFocusContext::Simulated, FireCount, [this]
	{
		SetFireCount(Selector, FireCount);
	}, [this]
	{
		FreshFireCore(Selector, FireCount, FireTimer, [this]
		{
			FireCallback(FireCount, FireTimer, EFocusContext::Simulated, [this]
			{
				return GunComponent->GetRemainBullets() <= 0;
			}, [this]
			{
				if (!Character->ReleaseFocus(EFocusContext::Simulated, EFocusSpace::MainHand, EFocusState::Firing))
					UE_LOG(LogNetSubObject, Error, TEXT("Fail to release focus on OnFireStartNotify!"));
			}, [this] { TraceVisualize(); });
		});
	});
}

void URiffleFireClient::OnFireStopNotify()
{
	Super::OnFireStopNotify();
	FireStopCore(Selector, FireCount, EFocusContext::Simulated);
}

void URiffleFireClient::OnRep_Character()
{
	Super::OnRep_Character();
	TraceQueryParams.ClearIgnoredActors();
	if (Character.IsValid()) TraceQueryParams.AddIgnoredActor(Character.Get());
}

void URiffleFireClient::OnCharacterDead()
{
	Super::OnCharacterDead();
	OnFireStopNotify();
}

void URiffleFireClient::TraceVisualize_Implementation()
{
	FHitResult HitResult;
	auto StartPoint = Character->GetActorLocation();
	
	// TODO : 나이아가라 이펙트 경로 지정
	UNiagaraSystem* NiagaraEffect =
	Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), nullptr,
		TEXT("/Game/Effects/M_VFX/VFX_GunImpact_01")));
	
	UNiagaraSystem* NiagaraBeamEffect =
	Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), nullptr,
		TEXT("/Game/Effects/M_VFX/VFX_GunTrail_Smoke")));

	UNiagaraSystem* NiagaraDecalEffect =
		Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), nullptr,
			TEXT("/Game/Effects/M_VFX/VFX_Decalimpact")));
	
	//BP_PlayerDummy 블루프린트 클래스에 [스켈레탈 메시 -> 스태틱 메시 -> 화살표 컴포넌트(Muzzle)] 의 위치를 받아 MuzzleLocation에 저장
	if (USkeletalMeshComponent* SkeletalMeshComp = Character->GetMesh())
	{
		if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(SkeletalMeshComp->GetChildComponent(0)))
		{
			if (UArrowComponent* ArrowComp = Cast<UArrowComponent>(StaticMeshComp->GetChildComponent(0)))
			{
				StartPoint = ArrowComp->GetComponentLocation();

				// TODO : 나이아가라 컴포넌트 가져와서 총구 위치에 이펙트 부착
				UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
						NiagaraEffect,
						ArrowComp,
						TEXT("VFX_GunImpact_01"),
						StartPoint,
						Character->GetActorRotation(),
						EAttachLocation::KeepWorldPosition,
						true);

				// auto CameraLocation = Character->GetCamera()->GetComponentLocation();
				// auto Distance = Character->GetSpringArm()->TargetArmLength;
				// auto Destination = CameraLocation + Character->GetCamera()->GetForwardVector() * (FireRange + Distance);
				// if (SqrFireRange < (HitResult.ImpactPoint - Character->GetActorLocation()).SquaredLength()) return;
				// if (!GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, Destination, ECC_Camera, TraceQueryParams))
				// 	return;
				//
				// auto BeamDestination = FVector((FireRange),0.0f,0.0f);
				// UNiagaraComponent* NiagaraBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				// 	GetWorld(),
				// 	NiagaraBeamEffect,
				// 	ArrowComp->GetComponentLocation(),
				// 	Character->GetActorRotation(),
				// 	FVector(1),
				// 	true,
				// 	true,
				// 	ENCPoolMethod::AutoRelease,
				// 	true);
				//
				// NiagaraBeam->SetVariableVec3(TEXT("BeamEnd"), BeamDestination);
				//
				// if (Cast<APawn>(HitResult.GetActor()) == nullptr)
				// {
				// 	UNiagaraComponent* NiagaraDecalImpact =
				// 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				// 	GetWorld(),
				// 	NiagaraDecalEffect,
				// 	HitResult.Location,
				// 	HitResult.Normal.Rotation());
				// }
				auto CameraLocation = Character->GetCamera()->GetComponentLocation();
				auto Distance = Character->GetSpringArm()->TargetArmLength;
				auto Direction = Character->GetCamera()->GetForwardVector();
				auto Destination = CameraLocation + Direction * (FireRange + Distance);
				FVector BeamEnd;

				if (!GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, Destination, ECC_Camera, TraceQueryParams))
				{
					BeamEnd = Destination;
				}
				else
				{
					BeamEnd = HitResult.Location;
				}

				UNiagaraComponent* NiagaraBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					NiagaraBeamEffect,
					ArrowComp->GetComponentLocation(),
					Character->GetActorRotation(),
					FVector(1),
					true,
					true,
					ENCPoolMethod::AutoRelease,
					true);
				NiagaraBeam->SetVariableVec3(TEXT("BeamEnd"), BeamEnd);

				if (Cast<APawn>(HitResult.GetActor()) == nullptr)
				{
					UNiagaraComponent* NiagaraDecalImpact = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						NiagaraDecalEffect,
						HitResult.Location,
						HitResult.Normal.Rotation());
				}
			}
		}
	}
}