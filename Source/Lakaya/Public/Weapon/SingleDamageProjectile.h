// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SingleDamageProjectile.generated.h"

DECLARE_EVENT_OneParam(ASingleDamageProjectile, FAttackEndingSignature, class ASingleDamageProjectile* const&)

UCLASS()
class LAKAYA_API ASingleDamageProjectile : public AActor
{
	GENERATED_BODY()

public:
	static const FName SphereComponentName;
	static const FName StaticMeshComponentName;
	static const FName MovementComponentName;

	explicit ASingleDamageProjectile(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	FORCEINLINE class USphereComponent* const& GetSphereComponent() const { return SphereComponent; }
	FORCEINLINE UStaticMeshComponent* const& GetStaticMeshComponent() const { return StaticMeshComponent; }
	FORCEINLINE class UProjectileMovementComponent* const& GetProjectileMovement() const { return MovementComponent; }

	// 투사체를 활성화합니다. 투사체가 즉시 활성화되어 공격을 시작합니다.
	virtual void EnableProjectile();

	// 투사체를 비활성화합니다. 투사체가 즉시 정지하고 충돌하지 않으며 보이지 않게 됩니다.
	virtual void DisableProjectile();

	virtual void SetDamage(const float& ArgDamage);

	// 투사체의 공격이 종료되는 시점에 호출됩니다. 매개변수로 이 투사체의 포인터를 받습니다.
	FAttackEndingSignature OnAttackEnded;

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovementComponent;

	float Damage;
};
