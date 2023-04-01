#pragma region 텍스츠 업데이트 주석



#pragma endregion

#include "UI/LoadingWidget.h"
#include "GameMode/IndividualGameMode.h"
#include "GameMode/IndividualGameState.h"
#include "Net/UnrealNetwork.h"

void ULoadingWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    AIndividualGameState* IndividualGameState = Cast<AIndividualGameState>(GetWorld()->GetGameState());
    if (IndividualGameState == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
        return;
    }
    
    InitMaxPlayers(IndividualGameState->GetMaxPlayers());
    OnChangeJoinedPlayers(IndividualGameState->NumPlayers);
    
    // 바인딩
    JoinedPlayersText = Cast<UTextBlock>(GetWidgetFromName(TEXT("JoinedPlayersText")));
    if (JoinedPlayersText == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
        return;
    }

    MaxPlayersText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MaxPlayersText")));
    if (MaxPlayersText == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_MaxPlayersText is null."));
        return;
    }
}

void ULoadingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    AIndividualGameState* IndividualGameState = Cast<AIndividualGameState>(GetWorld()->GetGameState());
    if (IndividualGameState == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
        return;
    }
    OnChangeJoinedPlayers(IndividualGameState->NumPlayers);
}

void ULoadingWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void ULoadingWidget::InitMaxPlayers(int32 MaxPlayers)
{
    MaxPlayersText->SetText(FText::FromString(FString::Printf(TEXT("%d)"), MaxPlayers)));
}

void ULoadingWidget::OnChangeJoinedPlayers(int32 JoinedPlayers)
{
    JoinedPlayersText->SetText(FText::FromString(FString::Printf(TEXT("(%d"), JoinedPlayers)));
}