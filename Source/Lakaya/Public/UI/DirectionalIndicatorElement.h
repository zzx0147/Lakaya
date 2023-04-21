#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Blueprint/UserWidget.h"
#include "Character/DamageableCharacter.h"
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
	virtual void IndicateStart(USceneComponent* NewMyPosition, FVector NewTargetPosition, float Time);
	virtual void IndicateStop();

private:
	class UImage* DirectionalIndicatorImage;
	FTimerHandle TimerHandleIndicate;
	USceneComponent* MyPosition;
	FVector TargetPosition;
	bool bIsIndicating;
};
