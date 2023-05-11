// Fill out your copyright notice in the Description page of Project Settings.
#define DO_CHECK 1

#include "UI/OccupationCharacterSelectWidget.h"
#include "Components/VerticalBox.h"
#include "UI/ImageTextWidget.h"
#include "Character/LakayaBasePlayerState.h"

UOccupationCharacterSelectWidget::UOccupationCharacterSelectWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UImageTextWidget> PlayerNameWidgetClassFinder(TEXT("/Game/Blueprints/UMG/WBP_PlayerNameWidget.WBP_PlayerNameWidget_C"));
	PlayerNameWidgetClass = PlayerNameWidgetClassFinder.Class;
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
	if (PlayerState != GetOwningPlayer()->GetPlayerState<APlayerState>())//로컬 플레이어가 아닐 때에만 따진다
	{
		//위젯을 생성하고 등록시킨다.
		const auto PlayerNameWidget = CreateWidget<UImageTextWidget>(this, PlayerNameWidgetClass);
		PlayerNameWidget->SetText(FText::FromString(PlayerState->GetPlayerName()));
		
		if (const auto BasePlayerState = Cast<ALakayaBasePlayerState>(PlayerState))
		{
			BasePlayerState->OnPlayerNameChanged.AddLambda([PlayerNameWidget](const FString& NewName){
					PlayerNameWidget->SetText(FText::FromString(NewName));});

			const auto CharacterName = BasePlayerState->GetCharacterName();
			if (CharacterSelectedPlayerListBoxMap.Contains(CharacterName))
			{
				CharacterSelectedPlayerListBoxMap[CharacterName]->AddChildToVerticalBox(PlayerNameWidget);
				BasePlayerState->OnCharacterNameChanged.AddLambda(
					[this, PlayerNameWidget](ALakayaBasePlayerState* ArgBasePlayerState, const FName& ArgCharacterName)
					{
						GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, *FString::Printf(TEXT("Change Character %s"), *ArgCharacterName.ToString()));
						CharacterSelectedPlayerListBoxMap[ArgCharacterName]->AddChildToVerticalBox(PlayerNameWidget);
						PlayerNameWidget->SetText(FText::FromString(ArgBasePlayerState->GetPlayerName()));
					}
				);
			}
		}
	}
}
