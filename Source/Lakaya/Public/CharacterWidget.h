// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE class USkillWidget* GetSkillWidget() const { return SkillWidget; }
	FORCEINLINE class UGamePlayBulletWidget* GetGamePlayBulletWidget() const { return BulletWidget; }

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillWidget> SkillWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGamePlayBulletWidget> BulletWidget;
};
