// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceComponent.h"
#include "FloatResourceComponent.generated.h"

DECLARE_EVENT_OneParam(UFloatResourceComponent, FFloatResourceSignature, const double&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UFloatResourceComponent : public UResourceComponent
{
	GENERATED_BODY()

public:
	UFloatResourceComponent();
	virtual void CostResource(const uint64& Value) override;
	virtual void CostResource(const double& Value) override;
	virtual void GainResource(const uint64& Value) override;
	virtual void GainResource(const double& Value) override;
	virtual bool IsEnough(const uint64& Value) const override;
	virtual bool IsEnough(const double& Value) const override;
	virtual bool IsFull() const override;
	virtual void Clear() override;
	virtual void Reset() override;

	// 최종적인 최대 자원수치를 가져옵니다.
	double GetMaxResource() const;

protected:
	UFUNCTION()
	virtual void OnRep_Resource();

public:
	// 현재 자원수치가 변경되면 호출됩니다.
	FFloatResourceSignature OnResourceUpdated;

	// 최대 자원수치가 변경되면 호출됩니다.
	FFloatResourceSignature OnMaxResourceUpdated;

protected:
	// 초기화시(시작, 부활, 캐릭터 변경 등)에 적용될 자원의 양을 지정합니다.
	UPROPERTY(EditAnywhere)
	double DefaultResource;

	// 자원의 기본 최대치를 지정합니다. 최종적으로 적용되는 최대치를 의미하지는 않습니다.
	UPROPERTY(EditAnywhere)
	double DefaultMaxResource;

	// true이면 음수까지 차감되는 것을 허용합니다. 
	UPROPERTY(EditAnywhere)
	uint8 bAllowNegative:1;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Resource)
	double Resource;
};
