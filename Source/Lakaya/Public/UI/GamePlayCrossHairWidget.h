#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayCrossHairWidget.generated.h"

class UImage;

//크로스헤어 표기 클래스
UCLASS()
class LAKAYA_API UGamePlayCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SetGamePlayCrosshairWidget(EIndividualGameState ChangeGameState);
private:
	UImage* CrosshairImage;//크로스헤어를 표기하는 위젯
};