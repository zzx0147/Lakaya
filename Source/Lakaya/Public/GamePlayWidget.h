// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayBulletWidget.h"
#include "GamePlayWidget.generated.h"

/**
 * 
 */

class UImage;
class UKillLogElement;
class UCanvasPanel;

UCLASS()
class LAKAYA_API UGamePlayWidget : public UGamePlayBulletWidget
{
	GENERATED_BODY()
	
public:
	UGamePlayWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

public:
	void SetCharacterPortrait(int32 CharacterNum);

protected:
	void OnChangeConsecutiveKills(int NewConsecutiveKills);


protected:
	UImage* CharacterPortraitImage;
	TArray<UTexture2D*> CharacterPortraitTextureArray;
	UCanvasPanel* HelpBodyPanel;
	TArray<UKillLogElement*> KillLogElementArray;
	TArray<UProgressBar*> SkillProgressBarArray;
	UProgressBar* ConsecutiveKillsProgressBar;
	int16 ConsecutiveKills;
	int16 MaximumConsecutiveKills;
private:
	
};
