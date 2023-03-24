#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayCrossHairWidget.generated.h"

class UImage;

//ũ�ν���� ǥ�� Ŭ����
UCLASS()
class LAKAYA_API UGamePlayCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayCrosshairWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UImage* CrosshairImage;//ũ�ν��� ǥ���ϴ� ����
};
