#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayConsecutiveKillsWidget.generated.h"

class UProgressBar;

UCLASS()
class LAKAYA_API UGamePlayConsecutiveKillsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayConsecutiveKillsWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	//연속 처치가 변경될 때 호출되는 함수(연결은 안되어있음)
	void OnChangeConsecutiveKills(int NewConsecutiveKills);

private:
	UProgressBar* ConsecutiveKillsProgressBar;//연속처치를 표기하는 프로그래스 바
	int16 ConsecutiveKills;//연속처치 횟수
	int16 MaximumConsecutiveKills;//연속처치 최대 횟수 
};
