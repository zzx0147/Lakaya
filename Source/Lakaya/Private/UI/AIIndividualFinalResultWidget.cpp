// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AIIndividualFinalResultWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "UI/AIIndividualFinalResultElement.h"

UAIIndividualFinalResultWidget::UAIIndividualFinalResultWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAIIndividualFinalResultWidget::SetMatchResultData(float KillScore,
	TArray<TWeakObjectPtr<ALakayaBasePlayerState>> PlayersByTeamMap)
{
	OnNotifyMatchResult(true);

	TArray<UAIIndividualFinalResultElement*> ElementArray;
	ElementArray.Emplace(AIIndividualFinalResultElement1);
	ElementArray.Emplace(AIIndividualFinalResultElement2);
	ElementArray.Emplace(AIIndividualFinalResultElement3);
	
	for (int32 i = 0; i < ElementArray.Num() && i < PlayersByTeamMap.Num(); ++i)
	{
		ElementArray[i]->SetPlayer(PlayersByTeamMap[i]);
		ElementArray[i]->SetGrade(i + 1);
		ElementArray[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UAIIndividualFinalResultWidget::SetVisibility(ESlateVisibility InVisibility)
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

void UAIIndividualFinalResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
