#include "UI/GameResultWidget.h"

#include "Character/LakayaBasePlayerState.h"
#include "GameMode/OccupationGameState.h"

//TODO: 지나치게 다른 클래스에 대한 의존도가 높습니다. 게임 결과만 표시하는데 굳이 게임스테이트나 플레이어스테이트까지 알아야할 필요가 있는지 모르겠습니다.
void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VictoryImage = Cast<UImage>(GetWidgetFromName(TEXT("Result_Victory_Image")));
	DefeatImage = Cast<UImage>(GetWidgetFromName(TEXT("Result_Defeat_Image")));
	AntiScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Result_Anti_Score_Text")));
	ProScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Result_Pro_Score_Text")));

	if (VictoryImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("VictoryImage is null."));
	if (DefeatImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("DefeatImage is null."));
	if (AntiScore == nullptr) UE_LOG(LogTemp, Warning, TEXT("AntiScore is null."));
	if (ProScore == nullptr) UE_LOG(LogTemp, Warning, TEXT("ProScore is null."));

	// OccupationGameState->OnChangeOccupationWinner.AddUObject(this, &UGameResultWidget::OnChangeWinner);
}

// void UGameResultWidget::OnChangeWinner(const EPlayerTeam& NewWinner)
// {
	// auto* OccupationPlayerState = Cast<ALakayaBasePlayerState>(GetOwningPlayer()->GetCharacter()->GetPlayerState());
	// if (OccupationPlayerState == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("OccupationPlayerState is null."));
	// 	return;
	// }
	//
	// auto WinTeam = OccupationGameState->GetOccupationWinner();
	// auto PlayerTeam = OccupationPlayerState->GetTeam();
	//
	// if (WinTeam != EPlayerTeam::Individual && PlayerTeam != EPlayerTeam::Individual && WinTeam == PlayerTeam)
	// 	GameResultWidgetText->SetText(FText::FromString(TEXT("승리!")));
	// else GameResultWidgetText->SetText(FText::FromString(TEXT("패배.")));
// }