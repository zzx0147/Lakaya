#define DO_CHECK 1

#include "UI/GamePlayPortraitWidget.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/Image.h"

UGamePlayPortraitWidget::UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static const ConstructorHelpers::FObjectFinder<UTexture2D> RenaPortraitFinder(
		TEXT("/Game/UI/CharacterPortrait/T_Rena"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> WaziPortraitFinder(
		TEXT("/Game/UI/CharacterPortrait/T_Wazi"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> GangrimPortraitFinder(
		TEXT("/Game/UI/CharacterPortrait/T_Citizen"));

	CharacterPortraitTextureMap.Reserve(3);

	if (RenaPortraitFinder.Succeeded()) CharacterPortraitTextureMap.Emplace(
		FName(TEXT("Rena")), RenaPortraitFinder.Object);
	if (WaziPortraitFinder.Succeeded()) CharacterPortraitTextureMap.Emplace(
		FName(TEXT("Wazi")), WaziPortraitFinder.Object);
	if (GangrimPortraitFinder.Succeeded()) CharacterPortraitTextureMap.Emplace(
		FName(TEXT("Gangrim")), GangrimPortraitFinder.Object);
}

void UGamePlayPortraitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널 체크
#pragma region InitAndNullCheck

	CharacterPortraitImage = Cast<UImage>(GetWidgetFromName(TEXT("CharacterPortrait_Img")));

	check(CharacterPortraitImage != nullptr);

#pragma endregion
}


// void UGamePlayPortraitWidget::BindCharacter(ACharacter* const& Character)
// {
	// const FName& CharacterName = Cast<ALakayaBaseCharacter>(Character)->GetCharacterName();

	// if (CharacterPortraitTextureMap.Contains(CharacterName))
	// 	CharacterPortraitImage->SetBrushFromTexture(CharacterPortraitTextureMap[CharacterName]);
// }


// bool UGamePlayPortraitWidget::UnbindCharacter(ACharacter* const& Character)
// {
	// FSlateBrush NoBrush;
	// NoBrush.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0));
	// CharacterPortraitImage->SetBrush(NoBrush);
	// return true;
// }
