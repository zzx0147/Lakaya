// Fill out your copyright notice in the Description page of Project Settings.


#include "AiCharacter.h"

#include "ArmedCharacter.h"

AAiCharacter::AAiCharacter() // 생성자
{

	
}

void AAiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// AI 동작 구현
}

// 콜리전 박스를 사용한 탐지
void AAiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AArmedCharacter::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		AArmedCharacter* PlayerCharacter = Cast<AArmedCharacter>(Actor);
		if (PlayerCharacter)
		{
			// 플레이어를 발견한 경우
			// 추적 시작
			break;
		}
	}
}

// 라인 트레이싱을 사용한 탐지
void AAiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * DetectionRange;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);
	if (bHit && HitResult.Actor->IsA(AArmedCharacter::StaticClass()))
	{
		// 플레이어를 발견한 경우
		// 추적 시작
	}
}