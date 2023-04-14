// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatSetupData.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_EVENT_OneParam(UStatComponent, FFloatSignature, const float&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatComponent();

	/**
	 * @brief 스텟 컴포넌트를 셋업합니다. 이 함수는 반드시 서버에서 호출되어야 합니다.
	 * @param RowName 스탯 컴포넌트 초기화 시 데이터 테이블에서 사용할 RowName입니다.
	 */
	void SetupStat(const FName& RowName);

	const float& GetWalkSpeed() const { return WalkSpeed; }
	float GetRunSpeed() const { return WalkSpeed + AdditiveRunSpeed; }
	const float& GetAdditiveRunSpeed() const { return AdditiveRunSpeed; }
	const float& GetMaximumHealth() const { return MaximumHealth; }

	/**
	 * @brief 걷기 속도를 영구적으로 증가시킵니다.
	 * @param Value 걷기 속도를 증가시킬 양입니다.
	 */
	void RegisterAdditionalWalkSpeed(const float& Value);

	/**
	 * @brief 걷기 속도를 일시적으로 증가시킵니다.
	 * @param Value 걷기 속도를 증가시킬 양입니다.
	 * @param Time 걷기 속도가 증가될 시간입니다.
	 */
	void RegisterAdditionalWalkSpeed(const float& Value, const float& Time);

	/**
	 * @brief 달리기 속도를 영구적으로 증가시킵니다.
	 * @param Value 달리기 속도를 증가시킬 양입니다.
	 */
	void RegisterAdditionalRunSpeed(const float& Value);

	/**
	 * @brief 달리기 속도를 일시적으로 증가시킵니다.
	 * @param Value 달리기 속도를 증가시킬 양입니다.
	 * @param Time 달리기 속도가 증가될 시간입니다.
	 */
	void RegisterAdditionalRunSpeed(const float& Value, const float& Time);

protected:
	virtual void SetupStatServer(const FStatSetupData* Data);

	UFUNCTION()
	virtual void OnRep_WalkSpeed();

	UFUNCTION()
	virtual void OnRep_RunSpeed();

	UFUNCTION()
	virtual void OnRep_MaximumHealth();

public:
	// 걷기 속도가 변경될 때 호출되는 이벤트입니다. 매개변수로 걷기속도를 받습니다.
	FFloatSignature OnWalkSpeedChanged;

	// 달리기 속도가 변경될 때 호출되는 이벤트입니다. 매개변수로 달리기 속도를 받습니다.
	FFloatSignature OnRunSpeedChanged;

	// 최대 체력이 변경될 때 호출되는 이벤트입니다. 매개변수로 최대 체력을 받습니다.
	FFloatSignature OnMaximumHealthChanged;

protected:
	UPROPERTY(EditAnywhere)
	UDataTable* StatSetupDataTable;

private:
	// 캐릭터의 걷기 속도를 의미합니다.
	UPROPERTY(Transient, ReplicatedUsing=OnRep_WalkSpeed)
	float WalkSpeed;

	// 캐릭터가 달릴 때, 걷기속도 대비하여 추가되는 이동 속도를 의미합니다.
	UPROPERTY(Transient, ReplicatedUsing=OnRep_RunSpeed)
	float AdditiveRunSpeed;

	// 캐릭터의 최대 체력을 나타냅니다.
	UPROPERTY(Transient, ReplicatedUsing=OnRep_MaximumHealth)
	float MaximumHealth;
};
