#include "UI/GameResultWidget.h"

#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/Character.h"
#include "GameMode/OccupationGameState.h"

// bool UGameResultWidget::OnMatchEnding()
// {
// 	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
// 	return true;
// }

//TODO: 지나치게 다른 클래스에 대한 의존도가 높습니다. 게임 결과만 표시하는데 굳이 게임스테이트나 플레이어스테이트까지 알아야할 필요가 있는지 모르겠습니다.
void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameResultWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameResultWidgetText")));
	if (GameResultWidgetText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
		return;
	}

	OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_GameMode is null."));
		return;
	}

	OccupationGameState->OnChangeOccupationWinner.AddUObject(this, &UGameResultWidget::OnChangeWinner);
}

void UGameResultWidget::OnChangeWinner(const EPlayerTeam& NewWinner)
{
	SetVisibility(ESlateVisibility::Visible);

	auto* OccupationPlayerState = Cast<ALakayaBasePlayerState>(GetOwningPlayer()->GetCharacter()->GetPlayerState());
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationPlayerState is null."));
		return;
	}

	auto WinTeam = OccupationGameState->GetOccupationWinner();
	auto PlayerTeam = OccupationPlayerState->GetTeam();

	if (WinTeam != EPlayerTeam::Individual && PlayerTeam != EPlayerTeam::Individual && WinTeam == PlayerTeam)
		GameResultWidgetText->SetText(FText::FromString(TEXT("승리!")));
	else GameResultWidgetText->SetText(FText::FromString(TEXT("패배.")));
}