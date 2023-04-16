#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DirectionalIndicatorElement.generated.h"

UCLASS()
class LAKAYA_API UDirectionalIndicatorElement : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UDirectionalIndicatorElement(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


public:
	virtual void IndicateStart(USceneComponent* NewMyPosition,const FVector& NewTargetPosition,const float& Time);
	virtual void IndicateStop();

private:
	TObjectPtr<class UImage> DirectionalIndicatorImage;
	FTimerHandle TimerHandleIndicate;
	TObjectPtr<USceneComponent> MyPosition;
	FVector TargetPosition;
	bool bIsIndicating;
};
