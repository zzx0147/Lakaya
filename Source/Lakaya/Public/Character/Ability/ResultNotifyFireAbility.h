// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "SimpleObjectPool.h"
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

DECLARE_EVENT_FourParams(UResultNotifyFireAbility, FSingleFireSignature, const FVector&, const FVector&, const FVector&,
                         const EFireResult&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UResultNotifyFireAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	UResultNotifyFireAbility();
	virtual bool ShouldStartRemoteCall() override;
	virtual bool ShouldStopRemoteCall() override;
	virtual void OnAliveStateChanged(const bool& AliveState) override;

protected:
	virtual void InitializeComponent() override;
	virtual void RemoteAbilityStart(const float& RequestTime) override;
	virtual void RemoteAbilityStop(const float& RequestTime) override;

public:
	// 현재 캐릭터의 사격의지를 가져옵니다.
	UFUNCTION(BlueprintCallable)
	const bool& IsWantsToFire() const { return bWantsToFire; }

protected:
	UFUNCTION()
	virtual void FireTick();
	virtual bool ShouldFire();
	virtual void SingleFire();
	virtual void FailToFire();
	virtual void ClearFireTimer();
	virtual float GetTerminalDamage(const FHitResult& HitResult);

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
	void DrawTrail(const FVector& Start, const FVector& End);
	void DrawImpact(const FVector& Location, const FVector& Normal, const EFireResult& Kind);

public:
	// 캐릭터가 사격을 실행한 후 호출됩니다. 매개변수로 사격 궤적 시작위치, 끝 위치, 충돌한 지점의 노멀벡터, 충돌한 물체의 종류를 받습니다.
	FSingleFireSignature OnSingleFire;

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

	// 총기 궤적을 그리는 나이아가라 시스템을 지정합니다.
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailNiagaraSystem;

	// 총구화염을 그리는 나이아가라 시스템을 지정합니다.
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* GunImpactSystem;

	// 어떤 물체에 사격이 적중한 경우 재생되는 나이아가라 시스템을 지정합니다.
	UPROPERTY(EditAnywhere)
	TMap<EFireResult, UNiagaraSystem*> ImpactNiagaraSystems;

	// 매 사격시에 소모될 자원들의 종류와 그 양을 지정합니다.
	UPROPERTY(EditAnywhere)
	TArray<FResourceCostData> FireCost;

	// 특정 부위 적중시 적용할 피해 배율을 정의합니다.
	UPROPERTY(EditAnywhere)
	TMap<FName, float> WeakPointMultiplier;

private:
	bool bWantsToFire;
	FTimerHandle FireTimer;
	FCollisionQueryParams CollisionQueryParams;
	TMap<EFireResult, TSimpleObjectPool<AActor>> DecalPool;
	TWeakObjectPtr<class UArrowComponent> MuzzleComponent;
};
