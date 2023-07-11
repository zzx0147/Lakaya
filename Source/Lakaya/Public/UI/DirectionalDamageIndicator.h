#pragma once

#include "CoreMinimal.h"
#include "UI/DirectionalIndicatorElement.h"
#include "DirectionalDamageIndicator.generated.h"


UCLASS()
class LAKAYA_API UDirectionalDamageIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	UDirectionalDamageIndicator(const FObjectInitializer& ObjectInitializer);

	// virtual void BindCharacter(ACharacter* const& Character) override;
	// virtual bool UnbindCharacter(ACharacter* const& Character) override;

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual void IndicateStart(const FString& CauserName, const FVector& DamageCursorPosition,const float& Damage);

private:
	TMap<FString, UDirectionalIndicatorElement*> IndicatorMap;

	TSubclassOf<UDirectionalIndicatorElement> IndicatorElementClass;

	TObjectPtr<class UCanvasPanel> IndicatorPanel;

	TObjectPtr<ACharacter> CharacterRef;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float IndicateTime;
};