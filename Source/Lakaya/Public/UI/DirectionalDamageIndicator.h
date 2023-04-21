#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/DirectionalIndicatorElement.h"
#include "DirectionalDamageIndicator.generated.h"


UCLASS()
class LAKAYA_API UDirectionalDamageIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	UDirectionalDamageIndicator(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual void IndicateStart(const FName& CauserName, USceneComponent* MyPosition, FVector DamageCursorPosition, float time);

private:
	TMap<FName, UDirectionalIndicatorElement*> IndicatorMap;

	TSubclassOf<UDirectionalIndicatorElement> IndicatorElementClass;

	class UCanvasPanel* IndicatorPanel;
};