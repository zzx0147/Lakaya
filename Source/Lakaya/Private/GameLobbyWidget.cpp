// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLobbyWidget.h"

UGameLobbyWidget::UGameLobbyWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UGameLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SelectedCharacterImage = Cast<UImage>(GetWidgetFromName(TEXT("Image_SelectedCharacter")));
	check(SelectedCharacterImage != nullptr);
}

void UGameLobbyWidget::ChangeSelectedCharacterImage(FString ImagePath)
{
	if (SelectedCharacterImage != nullptr)
	{
		
		UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, *ImagePath);

	}
}
