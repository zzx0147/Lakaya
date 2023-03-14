// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFireClient.h"

#include "GunComponent.h"
#include "ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

void URiffleFireClient::FireStart()
{
	Super::FireStart();
}

void URiffleFireClient::FireStop()
{
	if (Selector == EGunSelector::Auto) RequestFireStop(GetServerTime());
}

void URiffleFireClient::OnFireStartNotify()
{
	Super::OnFireStartNotify();
	FireStartCore(SelectorTimer, FireTimer,
	              nullptr,
	              [this] { NestedFireCore(Selector, FireCount); },
	              [this]
	              {
		              FreshFireCore(Selector, FireCount, FireTimer,
		                            [this]
		                            {
			                            FireCallback(FireCount, FireTimer,
			                                         [this]
			                                         {
				                                         //TODO: 생각보다 총알이 빠르게 리플리케이트 된 경우 1발정도 차이가 생길 수 있습니다.
				                                         return GunComponent.IsValid() &&
					                                         GunComponent->GetRemainBullets() <= 0;
			                                         },
			                                         nullptr,
			                                         [this] { TraceVisualize(); });
		                            });
	              });
}

void URiffleFireClient::OnFireStopNotify()
{
	Super::OnFireStopNotify();
	FireStopCore(Selector, FireCount);
}

void URiffleFireClient::OnSwitchSelectorNotify()
{
	Super::OnSwitchSelectorNotify();
	SwitchSelectorCore(FireCount, DesiredSelector, SelectorTimer,
	                   [this] { UpdateSelector(DesiredSelector, Selector); });
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
	//TODO: 추후 총기 이펙트의 시작지점은 총구위치가 되도록 변경해야 합니다.
	auto StartPoint = Character->GetActorLocation();
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
