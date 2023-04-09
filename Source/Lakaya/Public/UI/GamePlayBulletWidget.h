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
	// 위젯이 표시하는 정보가, 해당 컴포넌트의 값이 변경됨에 따라 변경되도록 합니다.
	void BindWeapon(class UGunComponent* const& GunComponent);

	// 해당 무기와의 바인딩을 해제합니다.
	void UnBindWeapon(UGunComponent* const& GunComponent);

private:
	void OnChangeRemainBullets(const uint16& NewRemainBullets);
	void OnChangeMagazineCapacity(const uint16& NewMagazineCapacity);

	UTextBlock* RemainBulletsText; //남은 총알을 표기하는 텍스트
	UTextBlock* MagazineCapacityText; //최대 총알을 표기하는 텍스트
};
