#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlaySkillWidget.generated.h"

class UProgressBar;

// 플레이 UI중 스킬 쿨타임을 표기하는 UI를 관리하는 클래스입니다
UCLASS()
class LAKAYA_API UGamePlaySkillWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlaySkillWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TArray<UProgressBar*> SkillProgressBarArray; //스킬 쿨타임을 표기하는 ProgressBar 배열입니다
};
