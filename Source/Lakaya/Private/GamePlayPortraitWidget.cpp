// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "GamePlayPortraitWidget.h"
#include "Components/Image.h"

UGamePlayPortraitWidget::UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGamePlayPortraitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterPortraitImage = Cast<UImage>(GetWidgetFromName(TEXT("CharacterPortrait_Img")));

	CharacterPortraitTextureArray =
	{
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_Citizen")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_GovernmentMan")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/CharacterPortrait/T_Gangster")),
	};

	check(CharacterPortraitImage != nullptr);
	for (auto temp : CharacterPortraitTextureArray) { check(temp != nullptr) }

	SetCharacterPortrait(0);
}

void UGamePlayPortraitWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeConstruct();
}

void UGamePlayPortraitWidget::SetCharacterPortrait(int32 CharacterNum)
{
	if (CharacterNum < 0 || CharacterPortraitTextureArray.Num() <= CharacterNum) return;

	CharacterPortraitImage->SetBrushFromTexture(CharacterPortraitTextureArray[CharacterNum]);
}