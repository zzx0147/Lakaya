#define DO_CHECK 1

#include "UI/GamePlayPortraitWidget.h"

#include "Components/Image.h"

UGamePlayPortraitWidget::UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static const ConstructorHelpers::FObjectFinder<UTexture2D> RenaPortraitFinder(TEXT(
		"/Script/Engine.Texture2D'/Game/UI/Charaterselect/T_InGame_UserInfo_CharImage_Rena.T_InGame_UserInfo_CharImage_Rena'"));

	static const ConstructorHelpers::FObjectFinder<UTexture2D> WaziPortraitFinder(TEXT(
		"/Script/Engine.Texture2D'/Game/UI/Charaterselect/T_InGame_UserInfo_CharImage_Wazi.T_InGame_UserInfo_CharImage_Wazi'"));

	static const ConstructorHelpers::FObjectFinder<UTexture2D> RenaWeaponFinder(
		TEXT("/Script/Engine.Texture2D'/Game/UI/Ingame/Rena/T_Rena_Weapon_Icon.T_Rena_Weapon_Icon'"));

	static const ConstructorHelpers::FObjectFinder<UTexture2D> WaziWeaponFinder(
		TEXT("/Script/Engine.Texture2D'/Game/UI/Ingame/wazi/T_Wazi_Weapon_Icon.T_Wazi_Weapon_Icon'"));

	PortraitTextureMap.Reserve(3);
	PortraitTextureMap.Emplace(FName(TEXT("Rena")), {RenaPortraitFinder.Object, RenaWeaponFinder.Object});
	PortraitTextureMap.Emplace(FName(TEXT("Wazi")), {WaziPortraitFinder.Object, WaziWeaponFinder.Object});
}

void UGamePlayPortraitWidget::ChangePortrait(const FName& Key)
{
	if (!PortraitTextureMap.Contains(Key)) return;
	const auto& [CharacterImage, WeaponImage] = PortraitTextureMap[Key];
	CharacterPortraitImage->SetBrushFromTexture(CharacterImage);
	WeaponPortraitImage->SetBrushFromTexture(WeaponImage);
}
