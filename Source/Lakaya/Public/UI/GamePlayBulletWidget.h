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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

private:
	void OnChangeRemainBullets(const uint16& NewRemainBullets);
	void OnChangeMagazineCapacity(const uint16& NewMagazineCapacity);
	void OnPrimaryWeaponChanged(class UWeaponComponent* const& WeaponComponent);

private:
	UTextBlock* RemainBulletsText; //남은 총알을 표기하는 텍스트
	UTextBlock* MagazineCapacityText; //최대 총알을 표기하는 텍스트
};
