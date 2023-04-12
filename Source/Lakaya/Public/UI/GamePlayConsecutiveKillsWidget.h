#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayConsecutiveKillsWidget.generated.h"

class UProgressBar;

//연속처치 표기 클래스
UCLASS()
class LAKAYA_API UGamePlayConsecutiveKillsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 해당 무기에 바인딩합니다.
	void BindWeapon(class UWeaponComponent* const& WeaponComponent);

	// 해당 무기와 바인딩을 해제합니다.
	void UnBindWeapon(UWeaponComponent* const& WeaponComponent);

protected:
	virtual void NativeConstruct() override;

	// 연속 처치가 변경될 때 호출되는 함수
	void OnChangeConsecutiveKills(const uint8& NewConsecutiveKills);

private:
	UProgressBar* ConsecutiveKillsProgressBar; //연속처치를 표기하는 프로그래스 바
	int16 ConsecutiveKills; //연속처치 횟수
	int16 MaximumConsecutiveKills; //연속처치 최대 횟수 
};
