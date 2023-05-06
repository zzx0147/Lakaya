// Fill out your copyright notice in the Description page of Project Settings.
#define DO_CHECK 1

#include "Components/VerticalBox.h"
#include "UI/ImageTextWidget.h"
#include "UI/OccupationCharacterSelectWidget.h"
#include "Character/LakayaBasePlayerState.h"

UOccupationCharacterSelectWidget::UOccupationCharacterSelectWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UImageTextWidget> PlayerNameWidgetClass(TEXT("/Game/Blueprints/UMG/WBP_PlayerNameWidget.WBP_PlayerNameWidget_C"));
	PlayerNameWidget = PlayerNameWidgetClass.Class;
}

void UOccupationCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterSelectedPlayerListBoxMap.Reserve(3);
	CharacterSelectedPlayerListBoxMap.Emplace(FName(TEXT("Rena")), Cast<UVerticalBox>(GetWidgetFromName(TEXT("Rena_SelectPlayerList_Pan"))));
	CharacterSelectedPlayerListBoxMap.Emplace(FName(TEXT("Wazi")), Cast<UVerticalBox>(GetWidgetFromName(TEXT("Wazi_SelectPlayerList_Pan"))));
	CharacterSelectedPlayerListBoxMap.Emplace(FName(TEXT("Minami")), Cast<UVerticalBox>(GetWidgetFromName(TEXT("Minami_SelectPlayerList_Pan"))));
	
	for (const auto temp : CharacterSelectedPlayerListBoxMap) check(temp.Value != nullptr);
}

void UOccupationCharacterSelectWidget::RegisterPlayer(APlayerState* PlayerState)
{
	Super::RegisterPlayer(PlayerState);

	const auto BasePlayerState = Cast<ALakayaBasePlayerState>(PlayerState);

	if (BasePlayerState != nullptr)
	{
		auto PlayerNameText = CreateWidget<UImageTextWidget>(this, PlayerNameWidget);
		PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
		PlayerNameText->SetVisibility(ESlateVisibility::Hidden);



	}
}

