// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RiffleFireClient.h"

#include "Weapon/GunComponent.h"
#include "Character/ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"

void URiffleFireClient::SetIsGPSFire_Implementation(bool bIsGPSFire)
{
	UCharAnimInstance* AnimInstance =
	Cast<UCharAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetIsGPSFire(bIsGPSFire);
	}
}

void URiffleFireClient::OnFireStartNotify()
{
	Super::OnFireStartNotify();
	SetIsGPSFire(true);
	FireStartCore(FireTimer,
	              [this]
	              {
		              return Character->SetFocus(EFocusContext::Simulated, EFocusSpace::MainHand, EFocusState::Firing);
	              },
	              [this] { ContinuousFireCore(Selector, FireCount); },
	              [this]
	              {
		              FreshFireCore(Selector, FireCount, FireTimer,
		                            [this]
		                            {
			                            FireCallback(FireCount, FireTimer,
			                                         [this] { return GunComponent->GetRemainBullets() <= 0; },
			                                         [this]
			                                         {
				                                         Character->ReleaseFocus(
					                                         EFocusContext::Simulated, EFocusSpace::MainHand,
					                                         EFocusState::Firing);
			                                         },
			                                         [this] { TraceVisualize(); },
			                                         [this]
			                                         {
				                                         Character->ReleaseFocus(
					                                         EFocusContext::Simulated, EFocusSpace::MainHand,
					                                         EFocusState::Firing);
			                                         });
		                            });
	              });
}

void URiffleFireClient::OnFireStopNotify()
{
	Super::OnFireStopNotify();
	SetIsGPSFire(false);
	FireStopCore(Selector, FireCount, true);
}

void URiffleFireClient::OnSwitchSelectorNotify()
{
	Super::OnSwitchSelectorNotify();
	SwitchSelectorCore(DesiredSelector, SelectorTimer,
	                   [this] { UpdateSelector(DesiredSelector, Selector, true); },
	                   [this]
	                   {
		                   return !Character->SetFocus(EFocusContext::Simulated, EFocusSpace::MainHand,
		                                               EFocusState::Switching);
	                   });
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
