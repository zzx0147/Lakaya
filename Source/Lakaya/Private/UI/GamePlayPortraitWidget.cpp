#define DO_CHECK 1

#include "UI/GamePlayPortraitWidget.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/Image.h"

UGamePlayPortraitWidget::UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static const ConstructorHelpers::FObjectFinder<UTexture2D> CitizenPortraitFinder(
		TEXT("/Game/UI/CharacterPortrait/T_Citizen"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> GovernmentManPortraitFinder(
		TEXT("/Game/UI/CharacterPortrait/T_GovernmentMan"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> GangsterPortraitFinder(
		TEXT("/Game/UI/CharacterPortrait/T_Gangster"));

	CharacterPortraitTextureMap.Reserve(3);

	if (CitizenPortraitFinder.Succeeded()) CharacterPortraitTextureMap.Emplace(
		FName(TEXT("Citizen")), CitizenPortraitFinder.Object);
	if (GovernmentManPortraitFinder.Succeeded())CharacterPortraitTextureMap.Emplace(
		FName(TEXT("GovernmentMan")), GovernmentManPortraitFinder.Object);
	if (GangsterPortraitFinder.Succeeded())CharacterPortraitTextureMap.Emplace(
		FName(TEXT("Gangster")), GangsterPortraitFinder.Object);
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
