#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponOutLineWidget.generated.h"

UCLASS()
class LAKAYA_API UWeaponOutLineWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TWeakObjectPtr<class UImage> WeaponOutLineImage;
};