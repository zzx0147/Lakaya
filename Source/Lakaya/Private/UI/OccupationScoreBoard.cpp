#include "UI/OccupationScoreBoard.h"

#include "Character/LakayaBasePlayerState.h"
#include "Components/VerticalBox.h"
#include "UI/ScoreBoardElement.h"

UOccupationScoreBoard::UOccupationScoreBoard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ElementClass = UScoreBoardElement::StaticClass();
}

void UOccupationScoreBoard::NativeConstruct()
{
	Super::NativeConstruct();

	AntiTeamBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("AntiTeamVerticalBox")));
	ProTeamBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("ProTeamVerticalBox")));

	if (!AntiTeamBox.IsValid()) UE_LOG(LogInit, Error, TEXT("UOccupationScoreBoard::AntiTeamBox was nullptr!"));
	if (!ProTeamBox.IsValid()) UE_LOG(LogInit, Error, TEXT("UOccupationScoreBoard::ProTeamBox was nullptr!"));
	if (!ElementClass) UE_LOG(LogInit, Error, TEXT("UOccupationScoreBoard::ElementClass was nullptr!"));
}

void UOccupationScoreBoard::RegisterPlayer(APlayerState* PlayerState)
{
	Super::RegisterPlayer(PlayerState);

	const auto LakayaState = Cast<ALakayaBasePlayerState>(PlayerState);
	const auto CharacterName = LakayaState->GetCharacterName().ToString();
	const auto Element = CreateWidget<UScoreBoardElement>(this, ElementClass);
	if (!LakayaState || !Element) return;
	UE_LOG(LogScript, Log, TEXT("UOccupationScoreBoard::RegisterPlayer Called"));

	switch (LakayaState->GetTeam())
	{
	case ETeam::Anti:
		if (AntiTeamBox.IsValid())
		{
			// TODO : 실행되지 않는 코드입니다.
			// ATeamBox->AddChildToVerticalBox(Element);
			// Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case ETeam::Pro:
		if (ProTeamBox.IsValid())
		{
			// TODO : 실행되지 않는 코드입니다.
			// BTeamBox->AddChildToVerticalBox(Element);
			// Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case ETeam::None:
		Element->AddToViewport();
		Element->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:
		UE_LOG(LogScript, Error, TEXT("Invalid team value on UOccupationScoreBoard::RegisterPlayer()!"));
		Element->RemoveFromRoot();
		return;
	}

	// 엘리먼트 데이터 바인딩
	LakayaState->OnTeamChanged.AddLambda([this, Element](const ETeam& ArgTeam)
	{
		// A팀으로 변경되면 B팀의 버티컬 박스에서 엘리먼트를 제거하고 A팀 버티컬 박스에 엘리먼트를 추가합니다.
		if (ArgTeam == ETeam::Anti)
		{
			if (ProTeamBox.IsValid())
				ProTeamBox->RemoveChild(Element);
			if (AntiTeamBox.IsValid() && !AntiTeamBox->HasChild(Element))
				AntiTeamBox->AddChildToVerticalBox(Element);
			
			Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Element->Anti_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		// B팀으로 변경되면 A팀의 버티컬 박스에서 엘리먼트를 제거하고 B팀 버티컬 박스에 엘리먼트를 추가합니다.
		else if (ArgTeam == ETeam::Pro)
		{
			if (AntiTeamBox.IsValid())
				AntiTeamBox->RemoveChild(Element);
			if (ProTeamBox.IsValid() && !ProTeamBox->HasChild(Element))
				ProTeamBox->AddChildToVerticalBox(Element);
			
			Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Element->Pro_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else UE_LOG(LogScript, Error, TEXT("Invalid team value on OnTeamChanged lambda handler!"));
	});

	LakayaState->OnCharacterNameChanged.AddLambda([Element](const ALakayaBasePlayerState* LakayaState, const FName& PlayerCharacterName)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnCharacterNameChanged."));

		if (PlayerCharacterName == TEXT("Rena"))
		{
			UE_LOG(LogTemp, Warning, TEXT("On_Rena"));
			Element->Character_Rena_Image->SetVisibility(ESlateVisibility::HitTestInvisible);
			Element->Character_Wazi_Image->SetVisibility(ESlateVisibility::Hidden);
		}
		if (PlayerCharacterName == TEXT("Wazi"))
		{
			UE_LOG(LogTemp, Warning, TEXT("On_Wazi"));
			Element->Character_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Element->Character_Rena_Image->SetVisibility(ESlateVisibility::Hidden);
		}
	});

	LakayaState->OnPlayerNameChanged.AddUObject(Element, &UScoreBoardElement::SetPlayerName);
	LakayaState->OnTotalScoreChanged.AddUObject(Element, &UScoreBoardElement::SetTotalScore);
	LakayaState->OnSuccessCaptureCountChanged.AddUObject(Element, &UScoreBoardElement::SetSuccessCaptureCount);
	LakayaState->OnKillCountChanged.AddUObject(Element, &UScoreBoardElement::SetKillCount);
	LakayaState->OnDeathCountChanged.AddUObject(Element, &UScoreBoardElement::SetDeathCount);

	Element->SetPlayerName(LakayaState->GetPlayerName());
	Element->SetTotalScore(LakayaState->GetTotalScore());
	Element->SetSuccessCaptureCount(LakayaState->GetSuccessCaptureCount());
	Element->SetDeathCount(LakayaState->GetDeathCount());
	Element->SetKillCount(LakayaState->GetKillCount());
}