// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameTimeWidget.h"

void UGameTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AIndividualGameState* IndividualGameState = Cast<AIndividualGameState>(GetWorld()->GetGameState());
	if (IndividualGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
		return;
	}

	OnChangeTime(IndividualGameState->GetMin(), IndividualGameState->GetSec());
	
	// 바인딩
	GameTimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameTimeWidget")));
	if (GameTimeText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameTimeText is null."));
		return;
	}

	// TODO : 시간이 바뀔 때마다 위젯 업데이트를 위한 델리게이트 등록
	IndividualGameState->OnChangeTime.AddUObject(this, &UGameTimeWidget::OnChangeTime);
}

void UGameTimeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGameTimeWidget::OnChangeTime(int32 Min, int32 Sec)
{
	FString MinuteString = FString::Printf(TEXT("%02d"), Min);
	FString SecondString = FString::Printf(TEXT("%02d"), Sec);
	FString TimeString = FString::Printf(TEXT("(%s:%s)"), *MinuteString, *SecondString);
	GameTimeText->SetText(FText::FromString(TimeString));
	// GameTimeText->SetText(FText::FromString(FString::Printf(TEXT("(%d:%d)"), Min, Sec)));
}

void UGameTimeWidget::ReMoveGameTimeWidget(EGameState ChangeGameState)
{
	// TODO : 게임 상태 조건
	this->RemoveFromParent();
}