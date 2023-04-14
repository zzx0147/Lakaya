#pragma once

#include "CoreMinimal.h"
#include "CharacterBindableWidget.h"
#include "UI/DirectionalIndicatorElement.h"
#include "DirectionalDamageIndicator.generated.h"


UCLASS()
class LAKAYA_API UDirectionalDamageIndicator : public UCharacterBindableWidget
{
	GENERATED_BODY()

public:
	UDirectionalDamageIndicator(const FObjectInitializer& ObjectInitializer);

	virtual void BindCharacter(ACharacter* const& Character) override;
	virtual bool UnbindCharacter(ACharacter* const& Character) override;

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