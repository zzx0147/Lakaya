#pragma once

#include "CoreMinimal.h"
#include "CharacterBindableWidget.h"
#include "GamePlayCrossHairWidget.generated.h"

class UImage;

//TODO: 아무 기능이 없는데 굳이 있어야 하는 클래스인가 싶습니다
//크로스헤어 표기 클래스
UCLASS()
class LAKAYA_API UGamePlayCrosshairWidget : public UCharacterBindableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UImage* CrosshairImage; //크로스헤어를 표기하는 위젯
};
