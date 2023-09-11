// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FinalResultElement.h"

#include "Character/LakayaBasePlayerState.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

UFinalResultElement::UFinalResultElement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CursorImageMap.Emplace(ETeam::Pro,nullptr);
	CursorImageMap.Emplace(ETeam::Anti,nullptr);
	UserInfoImageMap.Emplace(ETeam::Pro,nullptr);
	UserInfoImageMap.Emplace(ETeam::Anti,nullptr);
	UserInfoOutlineImageMap.Emplace(ETeam::Pro,nullptr);
	UserInfoOutlineImageMap.Emplace(ETeam::Anti,nullptr);
}

void UFinalResultElement::SetGrade(const uint32& NewGrade)
{
	static FText FormatText = FText::FromString(TEXT("{0}위"));

	GradeText->SetText(FText::Format(FormatText,NewGrade));
}

void UFinalResultElement::SetTeam(const ETeam& NewTeam)
{
	CursorImage->SetBrushFromTexture(CursorImageMap[NewTeam]);
	UserInfoBoxImage->SetBrushFromTexture(UserInfoImageMap[NewTeam]);
	UserInfoBoxOutlineImage->SetBrushFromTexture(UserInfoOutlineImageMap[NewTeam]);
}

void UFinalResultElement::SetPlayer(TObjectPtr<ALakayaBasePlayerState> NewPlayer)
{
	if(!NewPlayer) return;

	if(APlayerController* PlayerController = NewPlayer->GetPlayerController(); PlayerController != nullptr && PlayerController->IsLocalController())
	{
		CursorImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		CursorImage->SetVisibility(ESlateVisibility::Visible);
	}
	
	ScoreText->SetText(FText::AsNumber(NewPlayer->GetTotalScore()));
	UserIdText->SetText(FText::FromString(NewPlayer->GetPlayerName()));
}

void UFinalResultElement::NativeConstruct()
{
	Super::NativeConstruct();
}
