#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayBulletWidget.generated.h"

class UTextBlock;

//총알 갯수 표기 클래스
UCLASS()
class LAKAYA_API UGamePlayBulletWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayBulletWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void OnChangeRemainBullets(int16 NewRemainBullets);
	void OnChangeMagazineCapacity(int16 NewMagazineCapacity);

private:
	UTextBlock* RemainBulletsText;//남은 총알을 표기하는 텍스트
	UTextBlock* MagazineCapacityText;//최대 총알을 표기하는 텍스트

	int16 RemainBullets;//남은 총알
	int16 MagazineCapacity;//최대 총알
};
