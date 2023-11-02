#pragma once

#include "CoreMinimal.h"
#include "UI/DirectionalIndicatorElement.h"
#include "DirectionalDamageIndicator.generated.h"


UCLASS()
class LAKAYA_API UDirectionalDamageIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UDirectionalDamageIndicator(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	virtual void IndicateStart(const FString& CauserName, const FVector& DamageCursorPosition);

private:
	UPROPERTY()
	TMap<FString, TObjectPtr<UDirectionalIndicatorElement>> IndicatorMap;

	TSubclassOf<UDirectionalIndicatorElement> IndicatorElementClass;

	TObjectPtr<class UCanvasPanel> IndicatorPanel;

	TObjectPtr<ACharacter> CharacterRef;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float IndicateTime;
};