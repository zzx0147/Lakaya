// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FinalResultWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/FinalResultElement.h"

UFinalResultWidget::UFinalResultWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MyTeam = ETeam::None;
	ButtonTextureMap.Emplace(ETeam::Anti, nullptr);
	ButtonTextureMap.Emplace(ETeam::Pro, nullptr);
	
	ButtonPressedTextureMap.Emplace(ETeam::Anti, nullptr);
	ButtonPressedTextureMap.Emplace(ETeam::Pro, nullptr);

	
}

void UFinalResultWidget::SetTeam(const ETeam& NewTeam)
{
	MyTeam = NewTeam;
	FinalResultElement1->SetTeam(NewTeam);
	FinalResultElement2->SetTeam(NewTeam);
	FinalResultElement3->SetTeam(NewTeam);

	FButtonStyle ButtonStyle = ExitButton->GetStyle();

	ButtonStyle.Normal.SetResourceObject(ButtonTextureMap[NewTeam]);
	ButtonStyle.Hovered.SetResourceObject(ButtonTextureMap[NewTeam]);
	ButtonStyle.Pressed.SetResourceObject(ButtonPressedTextureMap[NewTeam]);
	ButtonStyle.Disabled.SetResourceObject(ButtonTextureMap[NewTeam]);
	
	ExitButton->SetStyle(ButtonStyle);
}

void UFinalResultWidget::SetMatchResultData(const ETeam WinTeam, const float ProScore, const float AntiScore, TMap<ETeam, TArray<TObjectPtr<ALakayaBasePlayerState>>> PlayersByTeamMap)
{
	FinalResultImage->SetBrushFromTexture(WinTeam == MyTeam ? VictoryResultTexture : DefeatResultTexture);
	ProScoreText->SetText(FText::AsNumber(ProScore));
	AntiScoreText->SetText(FText::AsNumber(AntiScore));

	TArray<UFinalResultElement*> ElementArray;
	ElementArray.Emplace(FinalResultElement1);
	ElementArray.Emplace(FinalResultElement2);
	ElementArray.Emplace(FinalResultElement3);
	
	if(PlayersByTeamMap.Contains(MyTeam))
	{
		for(int32 i = 0; i < PlayersByTeamMap[MyTeam].Num() && i < ElementArray.Num(); ++i)
		{
			ElementArray[i]->SetPlayer(PlayersByTeamMap[MyTeam][i]);
			ElementArray[i]->SetGrade(i + 1);
			ElementArray[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void UFinalResultWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (GetOwningPlayer() != nullptr &&
		(InVisibility == ESlateVisibility::Visible ||
		InVisibility == ESlateVisibility::HitTestInvisible ||
		InVisibility == ESlateVisibility::SelfHitTestInvisible))
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayer(),this);
		GetOwningPlayer()->SetShowMouseCursor(true);
	}
}

void UFinalResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
