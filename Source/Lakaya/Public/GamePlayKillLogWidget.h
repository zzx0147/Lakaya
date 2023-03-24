#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayKillLogWidget.generated.h"

class UKillLogElement;

//플레이 UI중 킬 로그를 표기하는 클래스입니다
UCLASS()
class LAKAYA_API UGamePlayKillLogWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayKillLogWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TArray<UKillLogElement*> KillLogElementArray;//킬 로그를 표기하는 엘리먼트 배열
};
