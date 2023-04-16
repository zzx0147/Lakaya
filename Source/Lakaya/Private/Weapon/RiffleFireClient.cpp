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

void URiffleFireClient::TraceVisualize()
{
	FHitResult HitResult;
	auto StartPoint = Character->GetActorLocation();
	
	static bool bPlayedNiagaraEffect = false;

	// TODO : 나이아가라 이펙트 경로 지정
	UNiagaraSystem* NiagaraEffect =
	Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), nullptr,
		TEXT("/Game/Effects/VFX_Character/VFX/Materials/Impacts/GunImpact/VFX/VFX_GunImpact_01")));
	UNiagaraSystem* NiagaraBeamEffect =
	Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), nullptr,
		TEXT("/Game/Effects/VFX_Character/VFX/Materials/Impacts/GunImpact/VFX/VFX_GunTrail_Smoke")));

	//BP_PlayerDummy 블루프린트 클래스에 [스켈레탈 메시 -> 스태틱 메시 -> 화살표 컴포넌트(Muzzle)] 의 위치를 받아 MuzzleLocation에 저장
	if (USkeletalMeshComponent* SkeletalMeshComp = Character->GetMesh())
		if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(SkeletalMeshComp->GetChildComponent(0)))
			if (UArrowComponent* ArrowComp = Cast<UArrowComponent>(StaticMeshComp->GetChildComponent(0)))
			{
				StartPoint = ArrowComp->GetComponentLocation();

				// TODO : 나이아가라 컴포넌트 가져와서 총구 위치에 이펙트 부착
				UNiagaraComponent* NiagaraComponent =
					UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffect, ArrowComp,
						TEXT("VFX_GunImpact_01"), StartPoint, Character->GetActorRotation(),
						EAttachLocation::KeepWorldPosition, true);
			}

	auto CameraLocation = Character->GetCamera()->GetComponentLocation();
	auto Distance = Character->GetSpringArm()->TargetArmLength;

	// FireRange는 캐릭터를 기준으로 정의된 값이므로 캐릭터와 카메라의 거리를 더해줍니다.
	auto Destination = CameraLocation + Character->GetCamera()->GetForwardVector() * (FireRange + Distance);

	// // Trace from camera
	// DrawDebugLine(GetWorld(), CameraLocation, Destination, FColor::Green, false, 1);
	// if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, Destination, ECC_Camera, TraceQueryParams))
	// 	Destination = HitResult.ImpactPoint;

	//DrawDebugLine(GetWorld(), StartPoint, Destination, FColor::Cyan, false, 1.f);

	// TODO : 총구 위치에서 사격했을때 궤적 그려주는 나이아가라 빔 이펙트 적용부분입니다
	if (USkeletalMeshComponent* SkeletalMeshComp = Character->GetMesh())
		if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(SkeletalMeshComp->GetChildComponent(0)))
			if(UArrowComponent* ArrowComp = Cast<UArrowComponent>(StaticMeshComp->GetChildComponent(0)))
			{
				auto BeamDestination = FVector((FireRange + Distance),0.0f,0.0f)/*ArrowComp->GetForwardVector() * */;
				UNiagaraComponent* NiagaraBeam = UNiagaraFunctionLibrary::
					SpawnSystemAtLocation(GetWorld(), NiagaraBeamEffect, StartPoint, Character->GetActorRotation());
				NiagaraBeam->SetVariableVec3(TEXT("BeamEnd"), BeamDestination);
				NiagaraBeam->SetVariableFloat(TEXT("Width"), BeamWidth);
				NiagaraBeam->SetVariableLinearColor(TEXT("Color"), BeamColor);
				NiagaraBeam->ActivateSystem(); 
			}

	// TODO : 총구위치에서 나이아가라 이펙트 스폰 시킨후 bPlayedNiagaraEffect = true 로 설정
	if (!bPlayedNiagaraEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraEffect, StartPoint, Character->GetActorRotation());
		bPlayedNiagaraEffect = true;
	}
}
