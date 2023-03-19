// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "GamePlayWidget.h"
#include "Components/Image.h"

#include "KillLogElement.h"
#include "DamageableCharacter.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

void UGamePlayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CharacterPortraitImage = Cast<UImage>(GetWidgetFromName("CharacterPortrait_Img"));

	CharacterPortraitTextureArray =
	{
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_Citizen")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_GovernmentMan")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_Gangster")),
	};

	KillLogElementArray =
	{
		Cast<UKillLogElement>(GetWidgetFromName("WBP_KillLogElement1")),
		Cast<UKillLogElement>(GetWidgetFromName("WBP_KillLogElement2")),
		Cast<UKillLogElement>(GetWidgetFromName("WBP_KillLogElement3"))
	};

	SkillProgressBarArray =
	{
		Cast<UProgressBar>(GetWidgetFromName("Skill1_Prb")),
		Cast<UProgressBar>(GetWidgetFromName("Skill2_Prb")),
	};


	HelpBodyPanel = Cast<UCanvasPanel>(GetWidgetFromName("HelpBodyPanel"));

	check(CharacterPortraitImage != nullptr);
	for (auto temp : CharacterPortraitTextureArray) { check(temp != nullptr) }
	for (auto temp : KillLogElementArray) { check(temp != nullptr) }






	SetCharacterPortrait(0);


	//Cast<ADamageableCharacter>(GetOwningPlayer())->OnHealthReplicated.Add()

}

void UGamePlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);
}

void UGamePlayWidget::SetCharacterPortrait(int32 CharacterNum)
{
	if (CharacterNum < 0 || CharacterPortraitTextureArray.Num() <= CharacterNum) return;


	CharacterPortraitImage->SetBrushFromTexture(CharacterPortraitTextureArray[CharacterNum]);
}
