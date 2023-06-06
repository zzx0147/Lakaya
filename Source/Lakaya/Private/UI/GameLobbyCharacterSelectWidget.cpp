#include "UI/GameLobbyCharacterSelectWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "UI/PlayerInfoWidget.h"

UGameLobbyCharacterSelectWidget::UGameLobbyCharacterSelectWidget(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RenderTargetRenaFinder(
		TEXT("/Game/Characters/RenderTarget/MI_Rena"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RenderTargetWaziFinder(
		TEXT("/Game/Characters/RenderTarget/MI_Wazi"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RenderTargetMinamiFinder(
		TEXT("/Game/Characters/RenderTarget/MI_Minami"));

	CharacterRenderTargetMaterialArray =
	{
		RenderTargetRenaFinder.Object,
		RenderTargetWaziFinder.Object,
		RenderTargetMinamiFinder.Object,
	};

	CharacterNameArray = {TEXT("Rena"), TEXT("Wazi"), TEXT("Minami")};
	for (auto temp : CharacterRenderTargetMaterialArray) { check(temp != nullptr) }

	CharacterIntroductionMap.Emplace(CharacterNameArray[0], FText::GetEmpty());
	CharacterIntroductionMap.Emplace(CharacterNameArray[1], FText::GetEmpty());
	CharacterIntroductionMap.Emplace(CharacterNameArray[2], FText::GetEmpty());

	GunTextureMap.Emplace(CharacterNameArray[0], nullptr);
	GunTextureMap.Emplace(CharacterNameArray[1], nullptr);
	GunTextureMap.Emplace(CharacterNameArray[2], nullptr);

	MagazineMap.Emplace(CharacterNameArray[0], 40);
	MagazineMap.Emplace(CharacterNameArray[1], 30);
	MagazineMap.Emplace(CharacterNameArray[2], 40);

	MagazineTextFormat = FText::FromString(TEXT("{0}/{0}"));
}

void UGameLobbyCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	SelectedCharacterImage = Cast<UImage>(GetWidgetFromName(TEXT("SelectedCharacter_Img")));

	CharacterButtonArray =
	{
		Cast<UButton>(GetWidgetFromName(FName(TEXT("Rena_Btn")))),
		Cast<UButton>(GetWidgetFromName(FName(TEXT("Wazi_Btn")))),
		Cast<UButton>(GetWidgetFromName(FName(TEXT("Minami_Btn"))))
	};

	IntroductionText = Cast<URichTextBlock>(GetWidgetFromName(TEXT("Introduction_Text")));

	GunImage = Cast<UImage>(GetWidgetFromName(TEXT("Gun_Img")));

	PlayerInfoWidget = Cast<UPlayerInfoWidget>(GetWidgetFromName(TEXT("MyPlayerInfo")));
	if (PlayerInfoWidget) PlayerInfoWidget->SetPlayerName(LocalPlayerName);

	MagazineInfoText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Magazine_Text")));

	check(SelectedCharacterImage != nullptr);
	for (auto temp : CharacterButtonArray) { check(temp != nullptr) }


#pragma endregion

	//버튼 함수 바인딩
	CharacterButtonArray[0]->OnClicked.AddUniqueDynamic(
		this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter1Button);
	CharacterButtonArray[1]->OnClicked.AddUniqueDynamic(
		this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter2Button);
	CharacterButtonArray[2]->OnClicked.AddUniqueDynamic(
		this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter3Button);

	//최초 캐릭터를 1번 캐릭터로 설정
	PrevCharacterButton = CharacterButtonArray[0];
	OnClickedCharacter1Button();
}

void UGameLobbyCharacterSelectWidget::SetLocalPlayerName(const FString& Name)
{
	LocalPlayerName = Name;
	if (PlayerInfoWidget) PlayerInfoWidget->SetPlayerName(LocalPlayerName);
}

void UGameLobbyCharacterSelectWidget::OnClickedCharacter1Button()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedCitizenButton")));
	SelectCharacter(0);
}

void UGameLobbyCharacterSelectWidget::OnClickedCharacter2Button()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedGovernmentManButton")));
	SelectCharacter(1);
}

void UGameLobbyCharacterSelectWidget::OnClickedCharacter3Button()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedGangsterButton")));
	SelectCharacter(2);
}

void UGameLobbyCharacterSelectWidget::SelectCharacter(const uint8& CharacterNum)
{
	if (!CharacterNameArray.IsValidIndex(CharacterNum)) return;
	//캐릭터를 선택하면 캐릭터 3D 출력을 변경하고 이전의 버튼을 활성화한뒤 선택한 캐릭터의 버튼을 비활성화
	//OnCharacterSelectedCharater로 변경된 캐릭터 번호를 브로드캐스트
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	                                 FString::Printf(TEXT("SelectCharacter %d"), CharacterNum));

	if (SelectedCharacterImage != nullptr &&
		CharacterRenderTargetMaterialArray.IsValidIndex(CharacterNum) &&
		CharacterRenderTargetMaterialArray[CharacterNum] != nullptr)
		SelectedCharacterImage->SetBrushFromMaterial(CharacterRenderTargetMaterialArray[CharacterNum]);

	PrevCharacterButton->SetIsEnabled(true);
	PrevCharacterButton = CharacterButtonArray[CharacterNum];
	PrevCharacterButton->SetIsEnabled(false);
	OnChangeSelectedCharacter.Broadcast(CharacterNameArray[CharacterNum]);

	if (IntroductionText != nullptr && CharacterIntroductionMap.Contains(CharacterNameArray[CharacterNum]))
		IntroductionText->SetText(CharacterIntroductionMap[CharacterNameArray[CharacterNum]]);

	if (GunImage != nullptr && GunTextureMap.Contains(CharacterNameArray[CharacterNum]) && GunTextureMap[
		CharacterNameArray[CharacterNum]] != nullptr)
		GunImage->SetBrushFromTexture(GunTextureMap[CharacterNameArray[CharacterNum]]);

	if (PlayerInfoWidget != nullptr)
		PlayerInfoWidget->SetCharacterName(CharacterNameArray[CharacterNum]);

	if (MagazineInfoText != nullptr && MagazineMap.Contains(CharacterNameArray[CharacterNum]))
		MagazineInfoText->SetText(FText::Format(MagazineTextFormat, MagazineMap[CharacterNameArray[CharacterNum]]));
}
