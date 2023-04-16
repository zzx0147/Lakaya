#pragma once

#include "CoreMinimal.h"
#include "CharacterBindableWidget.h"
#include "GamePlayHealthWidget.generated.h"

class UTextBlock;
class UProgressBar;

//체력을 표기하는 클래스
UCLASS()
class LAKAYA_API UGamePlayHealthWidget : public UCharacterBindableWidget
{
	GENERATED_BODY()

public:
	virtual void BindCharacter(ACharacter* const& Character) override;
	virtual bool UnbindCharacter(ACharacter* const& Character) override;

protected:
	virtual void NativeConstruct() override;

protected:
	//DamageableCharacter의 체력 델리게이트에 등록되는 함수, 체력이 변경될 때 호출
	void OnChangeHealth(const float& NewHealth);

	//DamageableCharacter의 최대 체력 델리게이트에 등록되는 함수, 최대 체력이 변경될 때 호출
	void OnChangeMaximumHealth(const float& NewMaximumHealth);

	//체력 프로그래스 바를 업데이트하는 함수
	void UpdateHealthProgressBar();

private:
	TObjectPtr<UProgressBar> HealthProgressBar; //체력을 표기하는 프로그래스 바
	TObjectPtr<UTextBlock> HealthText; //체력을 표기하는 텍스트
	TObjectPtr<UTextBlock> MaximumHealthText; //최대 체력을 표기하는 텍스트

	float MaximumHealth; //최대 체력 값
	float Health; //체력 값
};
