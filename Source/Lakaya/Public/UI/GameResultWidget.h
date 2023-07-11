#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

UCLASS()
class LAKAYA_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	TWeakObjectPtr<class UImage> VictoryImage;
	TWeakObjectPtr<UImage> DefeatImage;
	TWeakObjectPtr<class UTextBlock> AntiScore;
	TWeakObjectPtr<UTextBlock> ProScore;
};