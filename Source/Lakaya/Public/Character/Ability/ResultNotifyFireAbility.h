// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleObjectPool.h"
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

	/**
	 * @brief 사격의 전체 과정을 서버와 모든 클라이언트에게 전파합니다.
	 * @param Start 사격이 시작된 지점입니다.
	 * @param End 사격이 종료된 지점입니다.
	 * @param Normal 사격이 도달한 표면의 법선벡터입니다. 만약 도달한 표면이 없다면 영벡터입니다.
	 * @param FireResult 사격의 결과를 나타냅니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void NotifySingleFire(const FVector& Start, const FVector& End, const FVector& Normal,
	                      const EFireResult& FireResult);

	void InvokeFireNotify(const FHitResult& HitResult);
	void DrawDecal(const FVector& Location, const FVector& Normal, const EFireResult& Kind);

protected:
	UPROPERTY(EditAnywhere)
	float FireDelay;

	UPROPERTY(EditAnywhere)
	float FirstFireDelay;

	UPROPERTY(EditAnywhere)
	float FireRange;

	UPROPERTY(EditAnywhere)
	float FireDamage;

	// 사격 궤적이 총구로부터 그려져야만 한다는 제약조건입니다. true이면 총구에서부터 그려지지만, 궤적이 벽을 뚫을 수도 있습니다.
	UPROPERTY(EditAnywhere)
	bool bShouldFireSmoothing;

	// 사격이 어떤 물체에 충돌했을 때 어떤 액터를 보여줄지 지정합니다. 지정되지 않는 경우 데칼이 그려지지 않습니다.
	UPROPERTY(EditAnywhere)
	TMap<EFireResult, TSubclassOf<AActor>> DecalClasses;

	// 초기 오브젝트 풀의 초기화 사이즈를 지정합니다.
	UPROPERTY(EditAnywhere)
	uint8 PoolCount;

	// 데칼이 몇초간 보여질지를 정의합니다.
	UPROPERTY(EditAnywhere)
	float DecalShowingTime;

private:
	bool bWantsToFire;
	TWeakObjectPtr<UCameraComponent> Camera;
	FTimerHandle FireTimer;
	FCollisionQueryParams CollisionQueryParams;
	TMap<EFireResult, TSimpleObjectPool<AActor>> DecalPool;
};
