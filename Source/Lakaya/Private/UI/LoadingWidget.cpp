#include "UI/LoadingWidget.h"

#include "PlayerController/MenuCallingPlayerController.h"
#include "GameMode/IndividualGameMode.h"
#include "GameMode/IndividualGameState.h"
#include "Net/UnrealNetwork.h"

void ULoadingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
    if (OccupationGameState == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_GameMode is null."));
        return;
    }

    OnChangeJoinedPlayers(OccupationGameState->GetNumPlayers(), OccupationGameState->GetMaxPlayers());

    // 바인딩
    LoadingWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("LoadingWidgetText")));
    if (LoadingWidgetText == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
        return;
    }

    OccupationGameState->OnOccupationChangeJoinedPlayers.AddUObject(this, &ULoadingWidget::OnChangeJoinedPlayers);
    OccupationGameState->OnOccupationChangeMaxPlayers.AddUObject(this, &ULoadingWidget::OnChangeMaxPlayers);
    OccupationGameState->OnOccupationChangeGameState.AddUObject(this, &ULoadingWidget::ReMoveLoadingWidget);
}

void ULoadingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void ULoadingWidget::OnChangeJoinedPlayers(uint8 JoinedPlayers, uint8 MaxPlayer)
{
    if (JoinedPlayers == MaxPlayer)
    {
        LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("곧 게임을 시작합니다."))));
        return;
    }
    
    LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), JoinedPlayers, MaxPlayer)));
}

void ULoadingWidget::ReMoveLoadingWidget(EOccupationGameState ChangeGamState)
{
    if (ChangeGamState == EOccupationGameState::Progress)
    {
        this->RemoveFromParent();
    }
}

void ULoadingWidget::OnChangeMaxPlayers(uint8 Number, uint8 MaxPlayers)
{
    // if (Number == MaxPlayers)
    // {
    //     LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("곧 게임을 시작합니다."))));
    //     return;
    // }
    
    LoadingWidgetText->SetText(FText::FromString(FString::Printf(TEXT("(%d / %d)"), Number, MaxPlayers)));
}