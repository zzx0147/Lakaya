// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceComponent.h"
#include "BulletComponent.generated.h"

DECLARE_EVENT_OneParam(UBulletComponent, FBulletSignature, const uint16&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UBulletComponent : public UResourceComponent
{
	GENERATED_BODY()

	template <class UserClass>
	using TMemberFunction = void (UserClass::*)();

public:
	UBulletComponent();

	
	
	const uint16& GetBullets() const { return Bullets; }
	const uint16& GetMaxBullets() const { return MaxBullets; }

	/**
	 * @brief 총알을 차감하고, 그에 따른 함수를 실행합니다.
	 * @param Value 총알을 차감할 양입니다.
	 * @param Caller 멤버함수를 실행할 객체입니다. 보통 this를 넘겨주면 됩니다.
	 * @param OnSuccess 차감에 성공하면 실행될 멤버함수입니다.
	 * @param OnFailed 차감에 실패하면 실행될 멤버함수입니다.
	 */
	template <class UserClass>
	void CostBullet(const uint16& Value, UserClass* const& Caller = nullptr,
	                TMemberFunction<UserClass>& OnSuccess = nullptr, TMemberFunction<UserClass>& OnFailed = nullptr);

	void Reload();

protected:
	UFUNCTION()
	virtual void OnRep_Bullets();

	UFUNCTION()
	virtual void OnRep_MaxBullets();

public:
	// 현재 총알이 변경되면 호출됩니다.
	FBulletSignature OnBulletsChanged;

	// 최대 총알 개수가 변경되면 호출됩니다.
	FBulletSignature OnMaxBulletsChanged;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Bullets)
	uint16 Bullets;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_MaxBullets)
	uint16 MaxBullets;
};

template <class UserClass>
void UBulletComponent::CostBullet(const uint16& Value, UserClass* const& Caller, TMemberFunction<UserClass>& OnSuccess,
                                  TMemberFunction<UserClass>& OnFailed)
{
	if (Bullets >= Value)
	{
		Bullets -= Value;
		if (Caller && OnSuccess) (Caller->*OnSuccess)();
		OnBulletsChanged.Broadcast(Bullets);
	}
	else if (Caller && OnFailed) (Caller->*OnFailed)();
}
