// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IntroElement.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

UIntroElement::UIntroElement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CharacterTextureMap.Emplace(TEXT("Rena"));
	CharacterTextureMap.Emplace(TEXT("Wazi"));
	CharacterTextureMap.Emplace(TEXT("Gangrim"));
	
}

void UIntroElement::SetPlayerData(bool IsLocal, const FName& CharacterName, const FString& PlayerName)
{
	if(IsLocal) BackgroundImage->SetBrushFromTexture(LocalBackgroundImage);

	if(CharacterTextureMap.Contains(CharacterName)) CharacterImage->SetBrushFromTexture(CharacterTextureMap[CharacterName]);
	
	PlayerNameText->SetText(FText::FromString(PlayerName));
}
