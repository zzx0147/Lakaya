#pragma once

#include "CoreMinimal.h"
#include "CharacterBindableWidget.h"
#include "GamePlayBulletWidget.generated.h"

class UTextBlock;

//총알 갯수 표기 클래스
UCLASS()
class LAKAYA_API UGamePlayBulletWidget : public UCharacterBindableWidget
{
	GENERATED_BODY()

public:
	virtual void BindCharacter(ACharacter* const& Character) override;
	virtual bool UnbindCharacter(ACharacter* const& Character) override;

protected:
	virtual void NativeConstruct() override;

private:
	void OnChangeRemainBullets(const uint16& NewRemainBullets) const;
	void OnChangeMagazineCapacity(const uint16& NewMagazineCapacity) const;

	UTextBlock* RemainBulletsText; //남은 총알을 표기하는 텍스트
	UTextBlock* MagazineCapacityText; //최대 총알을 표기하는 텍스트
};
