#pragma once
#define DO_CHECK 1

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayBulletWidget.generated.h"

struct FOnAttributeChangeData;
class UTextBlock;

//총알 갯수 표기 클래스
UCLASS()
class LAKAYA_API UGamePlayBulletWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UGamePlayBulletWidget(const FObjectInitializer& ObjectInitializer);
	
	void SetRemainBullet(const uint16& NewRemainBullet);
	void SetMaxBullet(const uint16& NewMaxBullet);

	void OnChangeMaxBulletAttribute(const FOnAttributeChangeData& NewValue);
	void OnChangeCurrentBulletAttribute(const FOnAttributeChangeData& NewValue);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBulletProgressBar(const float& NewPercent);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RemainBulletsText; //남은 총알을 표기하는 텍스트

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MagazineCapacityText; //최대 총알을 표기하는 텍스트

	float MaxBullet;

	float RemainBullet;
};
