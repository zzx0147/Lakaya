// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "SkillWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API USkillWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	TWeakObjectPtr<UImage> RenaQSkill;			// 레나 Q 스킬
	TWeakObjectPtr<UImage> RenaESkill;			// 레나 E 스킬
	TWeakObjectPtr<UImage> RenaRButtonSkill;	// 레나 우클릭 스킬
	// TWeakObjectPtr<UImage> RenaRSkill;			// 레나 R 스킬

	TWeakObjectPtr<UImage> WaziQSkill;			// 와지 Q 스킬
	TWeakObjectPtr<UImage> WaziESkill;			// 와지 E 스킬
	TWeakObjectPtr<UImage> WaziRButtonSkill;	// 와지 우클릭 스킬
	// TWeakObjectPtr<UImage> WaziRSkill;			// 와지 R 스킬
};