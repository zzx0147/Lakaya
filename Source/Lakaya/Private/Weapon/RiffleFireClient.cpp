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

void URiffleFireClient::OnSwitchSelectorNotify()
{
	Super::OnSwitchSelectorNotify();
	SwitchSelectorCore(DesiredSelector, Selector, SelectorTimer, EFocusContext::Simulated);
}

void URiffleFireClient::OnRep_Character()
{
	Super::OnRep_Character();
	TraceQueryParams.ClearIgnoredActors();
	if (Character.IsValid()) TraceQueryParams.AddIgnoredActor(Character.Get());
}

void URiffleFireClient::TraceVisualize()
{
	FHitResult HitResult;
	auto StartPoint = Character->GetActorLocation();

	//BP_PlayerDummy 블루프린트 클래스에 [스켈레탈 메시 -> 스태틱 메시 -> 화살표 컴포넌트(Muzzle)] 의 위치를 받아 MuzzleLocation에 저장
	if (USkeletalMeshComponent* SkeletalMeshComp = Character->GetMesh())
		if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(SkeletalMeshComp->GetChildComponent(0)))
			if (UArrowComponent* ArrowComp = Cast<UArrowComponent>(StaticMeshComp->GetChildComponent(0)))
				StartPoint = ArrowComp->GetComponentLocation();

	auto CameraLocation = Character->GetCamera()->GetComponentLocation();
	auto Distance = Character->GetSpringArm()->TargetArmLength;

	// FireRange는 캐릭터를 기준으로 정의된 값이므로 캐릭터와 카메라의 거리를 더해줍니다.
	auto Destination = CameraLocation + Character->GetCamera()->GetForwardVector() * (FireRange + Distance);

	// Trace from camera
	DrawDebugLine(GetWorld(), CameraLocation, Destination, FColor::Green, false, 1);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, Destination, ECC_Camera, TraceQueryParams))
		Destination = HitResult.ImpactPoint;

	DrawDebugLine(GetWorld(), StartPoint, Destination, FColor::Cyan, false, 1.f);
}
