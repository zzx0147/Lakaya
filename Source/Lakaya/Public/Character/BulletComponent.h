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

protected:
	virtual void OnRegister() override;

public:
	const uint16& GetBullets() const { return Bullets; }

	//TODO: StatPlayerState에서 연관된 스탯을 참조하여 최대총알 개수가 증가되도록 변경해야 합니다.
	const uint16& GetMaxBullets() const { return MaxBullets; }

	// 탄창이 가득차있는지 여부를 조사합니다.
	bool IsFull() const { return GetBullets() == GetMaxBullets(); }

	/**
	 * @brief 총알을 차감하고, 그에 따른 함수를 실행합니다.
	 * @param Value 총알을 차감할 양입니다.
	 * @param Caller 멤버함수가 실행될 객체입니다. 보통 this를 넘겨주면 됩니다.
	 * @param OnSuccess 차감에 성공하면 실행될 멤버함수입니다.
	 * @param OnFailed 차감에 실패하면 실행될 멤버함수입니다.
	 */
	template <class UserClass = nullptr_t>
	void CostBullet(const uint16& Value, UserClass* const& Caller = nullptr,
	                TMemberFunction<UserClass> OnSuccess = nullptr, TMemberFunction<UserClass> OnFailed = nullptr);

	/**
	 * @brief 총알 차감을 시도하고, 결과를 반환합니다.
	 * @param Value 총알을 차감할 양입니다.
	 * @return true이면 차감에 성공했음을 나타내며, 그렇지 않으면 실패했음을 나타냅니다.
	 */
	bool CostBullet(const uint16& Value);

	// 탄창을 다시 최대 탄약까지 채워넣습니다.
	void Reload();

protected:
	UFUNCTION()
	virtual void OnRep_Bullets();

	UFUNCTION()
	virtual void OnRep_MaxBullets();

	UFUNCTION()
	virtual void OnOwnerControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	// 총알 개수를 표시하는 위젯을 셋업합니다.
	virtual void SetupBulletWidget(APlayerController* LocalController);

	// 총알 개수를 표시하는 위젯을 제거합니다.
	virtual void RemoveBulletWidget();

public:
	// 현재 총알이 변경되면 호출됩니다.
	FBulletSignature OnBulletsChanged;

	// 최대 총알 개수가 변경되면 호출됩니다.
	FBulletSignature OnMaxBulletsChanged;

protected:
	// 최대 탄창 크기를 지정합니다. 최종적인 탄창 크기는 아니며, 버프로 증가되는 탄창 크기는 별도로 계산됩니다.
	UPROPERTY(EditAnywhere)
	uint16 MaxBullets;

	// 총알 개수를 표시하는 위젯의 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGamePlayBulletWidget> BulletWidgetClass;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Bullets, Transient)
	uint16 Bullets;

	TWeakObjectPtr<UGamePlayBulletWidget> BulletWidget;
};

template <class UserClass>
void UBulletComponent::CostBullet(const uint16& Value, UserClass* const& Caller, void (UserClass::*OnSuccess)(),
                                  void (UserClass::*OnFailed)())
{
	if (Bullets >= Value)
	{
		Bullets -= Value;
		if (Caller && OnSuccess) (Caller->*OnSuccess)();
		OnBulletsChanged.Broadcast(Bullets);
	}
	else if (Caller && OnFailed) (Caller->*OnFailed)();
}
