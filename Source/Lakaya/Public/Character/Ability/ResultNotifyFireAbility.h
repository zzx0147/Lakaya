// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StopRemoteCallAbility.h"
#include "ResultNotifyFireAbility.generated.h"

UENUM()
enum class EFireResult
{
	// 충돌하지 못했음을 의미합니다.
	None,

	// 다른 배경 표면에 충돌했음을 의미합니다.
	Environment,

	// 생명체에 충돌했음을 의미합니다.
	Creature
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UResultNotifyFireAbility : public UStopRemoteCallAbility
{
	GENERATED_BODY()

public:
	UResultNotifyFireAbility();
	virtual void AbilityStart() override;
	virtual void AbilityStop() override;

protected:
	virtual void InitializeComponent() override;
	virtual void RequestStart_Implementation(const float& RequestTime) override;
	virtual void RequestStop_Implementation(const float& RequestTime) override;

	UFUNCTION()
	virtual void FireTick();
	virtual bool ShouldFire();
	virtual void SingleFire();
	virtual void FailToFire();
	virtual void ClearFireTimer();

private:
	/**
	 * @brief 사격의 결과를 서버와 모든 클라이언트에게 전파합니다.
	 * @param HitPoint 사격이 어디에 도달했는지를 나타냅니다.
	 * @param Normal 사격이 도달한 표면의 월드공간의 법선벡터입니다.
	 * @param FireResult 사격이 어디에 부딫혔는지를 나타냅니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void NotifyFireResult(const FVector& HitPoint, const FVector& Normal, const EFireResult& FireResult);

protected:
	UPROPERTY(EditAnywhere)
	float FireDelay;

	UPROPERTY(EditAnywhere)
	float FirstFireDelay;

	UPROPERTY(EditAnywhere)
	float FireRange;

	UPROPERTY(EditAnywhere)
	float FireDamage;

private:
	bool bWantsToFire;
	TWeakObjectPtr<class UCameraComponent> Camera;
	FTimerHandle FireTimer;
	FCollisionQueryParams CollisionQueryParams;
};
