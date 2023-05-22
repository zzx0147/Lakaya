#include "UI/PlayerInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

UPlayerInfoWidget::UPlayerInfoWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	CharacterPortraitMap.Emplace(FName(TEXT("Rena")));
	CharacterPortraitMap.Emplace(FName(TEXT("Wazi")));
	CharacterPortraitMap.Emplace(FName(TEXT("Minami")));
	PlayerName = FText::GetEmpty();
}

void UPlayerInfoWidget::SetPlayerName(FString NewName)
{
	PlayerName = FText::FromString(NewName);
	if(PlayerNameText != nullptr)
		PlayerNameText->SetText(PlayerName);
}

void UPlayerInfoWidget::SetCharacterName(const FName& NewName)
{
	CharacterNameText->SetText(FText::FromName(NewName));

	if(CharacterPortraitMap.Contains(NewName))
		CharacterPortraitImage->SetBrushFromTexture(CharacterPortraitMap[NewName]);
}

void UPlayerInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	PlayerNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerName_Text")));
	CharacterNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CharacterName_Text")));
	CharacterPortraitImage = Cast<UImage>(GetWidgetFromName(TEXT("Character_Img")));

	if(PlayerNameText != nullptr)
		PlayerNameText->SetText(PlayerName);
}
