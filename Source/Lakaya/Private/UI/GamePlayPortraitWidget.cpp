#define DO_CHECK 1

#include "UI/GamePlayPortraitWidget.h"

#include "Components/Image.h"

UGamePlayPortraitWidget::UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static const ConstructorHelpers::FObjectFinder<UTexture2D> RenaPortraitFinder(TEXT(
		"/Script/Engine.Texture2D'/Game/UI/Charaterselect/T_InGame_UserInfo_CharImage_Rena.T_InGame_UserInfo_CharImage_Rena'"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> WaziPortraitFinder(TEXT(
		"/Script/Engine.Texture2D'/Game/UI/Charaterselect/T_InGame_UserInfo_CharImage_Wazi.T_InGame_UserInfo_CharImage_Wazi'"));

	CharacterPortraitTextureMap.Reserve(3);
	CharacterPortraitTextureMap.Emplace(FName(TEXT("Rena")), RenaPortraitFinder.Object);
	CharacterPortraitTextureMap.Emplace(FName(TEXT("Wazi")), WaziPortraitFinder.Object);
}

void UGamePlayPortraitWidget::ChangePortrait(const FName& Key)
{
	if (!CharacterPortraitImage || !CharacterPortraitTextureMap.Contains(Key)) return;
	CharacterPortraitImage->SetBrushFromTexture(CharacterPortraitTextureMap[Key]);
}
