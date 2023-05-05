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

protected:
	virtual void NativeConstruct() override;

public:
	void SetRemainBullet(const uint16& RemainBullet);
	void SetMaxBullet(const uint16& MaxBullet);

private:
	TWeakObjectPtr<UTextBlock> RemainBulletsText; //남은 총알을 표기하는 텍스트
	TWeakObjectPtr<UTextBlock> MagazineCapacityText; //최대 총알을 표기하는 텍스트
};
