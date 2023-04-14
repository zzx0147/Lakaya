#pragma once

#include "CoreMinimal.h"
#include "CharacterBindableWidget.h"
#include "GamePlayConsecutiveKillsWidget.generated.h"

class UProgressBar;

//연속처치 표기 클래스
UCLASS()
class LAKAYA_API UGamePlayConsecutiveKillsWidget : public UCharacterBindableWidget
{
	GENERATED_BODY()

public:
	virtual void BindCharacter(ACharacter* const& Character) override;
	virtual bool UnbindCharacter(ACharacter* const& Character) override;

protected:
	virtual void NativeConstruct() override;

	// 연속 처치가 변경될 때 호출되는 함수
	void OnChangeConsecutiveKills(const uint8& NewConsecutiveKills);

private:
	UProgressBar* ConsecutiveKillsProgressBar; //연속처치를 표기하는 프로그래스 바
	int16 ConsecutiveKills; //연속처치 횟수
	int16 MaximumConsecutiveKills; //연속처치 최대 횟수 
};
