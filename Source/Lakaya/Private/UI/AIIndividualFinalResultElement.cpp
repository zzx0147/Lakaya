// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AIIndividualFinalResultElement.h"

#include "Character/LakayaBasePlayerState.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

UAIIndividualFinalResultElement::UAIIndividualFinalResultElement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Character3DImageMaterialMap.Emplace( FName(TEXT("Rena")),nullptr);
	Character3DImageMaterialMap.Emplace( FName(TEXT("Wazi")),nullptr);
	Character3DImageMaterialMap.Emplace( FName(TEXT("Gangrim")),nullptr);

	GradeImageMap.Emplace(FName(TEXT("1")),nullptr);
	GradeImageMap.Emplace(FName(TEXT("2")),nullptr);
	GradeImageMap.Emplace(FName(TEXT("3")),nullptr);
	GradeImageMap.Emplace(FName(TEXT("4")),nullptr);
	GradeImageMap.Emplace(FName(TEXT("5")),nullptr);
	GradeImageMap.Emplace(FName(TEXT("6")),nullptr);
}

void UAIIndividualFinalResultElement::SetGrade(const uint32& NewGrade)
{
	static FText FormatText = FText::FromString(TEXT("{0}위"));

	GradeText->SetText(FText::Format(FormatText,NewGrade));
	
	if(GradeImageMap.Contains(FName(*FString::FromInt(NewGrade))))
		GradeImage->SetBrushFromTexture(GradeImageMap[FName(*FString::FromInt(NewGrade))]);
	else
		GradeImage->SetBrushFromTexture(nullptr);
}

void UAIIndividualFinalResultElement::SetPlayer(TWeakObjectPtr<ALakayaBasePlayerState> NewPlayer)
{
	if(!NewPlayer.IsValid()) return;

	if(const APlayerController* PlayerController = NewPlayer->GetPlayerController(); PlayerController != nullptr && PlayerController->IsLocalPlayerController())
	{
		CursorImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		CursorImage->SetVisibility(ESlateVisibility::Hidden);
	}
	
	KillScoreText->SetText(FText::AsNumber(NewPlayer->GetKillCount()));
	UserIdText->SetText(FText::FromString(NewPlayer->GetPlayerName()));

	if(Character3DImageMaterialMap.Contains(NewPlayer->GetCharacterName()))
	{
		Character3DImage->SetBrushFromTexture(Character3DImageMaterialMap[NewPlayer->GetCharacterName()]);
	}
}

void UAIIndividualFinalResultElement::NativeConstruct()
{
	Super::NativeConstruct();
}
