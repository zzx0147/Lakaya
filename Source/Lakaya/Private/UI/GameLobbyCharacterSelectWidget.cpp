#include "UI/GameLobbyCharacterSelectWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"

UGameLobbyCharacterSelectWidget::UGameLobbyCharacterSelectWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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

	CharacterNameArray = { TEXT("Rena"), TEXT("Wazi"), TEXT("Minami") };

	CharacterRenderTargetMaterialArray =
	{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_Rena")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_Wazi")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_Minami"))
	};

	check(SelectedCharacterImage != nullptr);
	for (auto temp : CharacterButtonArray) { check(temp != nullptr) }
	for (auto temp : CharacterRenderTargetMaterialArray) { check(temp != nullptr) }

#pragma endregion 

	//버튼 함수 바인딩
	CharacterButtonArray[0]->OnClicked.AddDynamic(this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter1Button);
	CharacterButtonArray[1]->OnClicked.AddDynamic(this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter2Button);
	CharacterButtonArray[2]->OnClicked.AddDynamic(this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter3Button);

	//최초 캐릭터를 1번 캐릭터로 설정
	PrevCharacterButton = CharacterButtonArray[0];
	OnClickedCharacter1Button();
}

void UGameLobbyCharacterSelectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGameLobbyCharacterSelectWidget::OnClickedCharacter1Button()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedCitizenButton")));
	SelectCharacter(0);
}

void UGameLobbyCharacterSelectWidget::OnClickedCharacter2Button()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedGovernmentManButton")));
	SelectCharacter(1);
}

void UGameLobbyCharacterSelectWidget::OnClickedCharacter3Button()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedGangsterButton")));
	SelectCharacter(2);
}

void UGameLobbyCharacterSelectWidget::SelectCharacter(const uint8& CharacterNum)
{
	//캐릭터를 선택하면 캐릭터 3D 출력을 변경하고 이전의 버튼을 활성화한뒤 선택한 캐릭터의 버튼을 비활성화
	//OnCharacterSelectedCharater로 변경된 캐릭터 번호를 브로드캐스트
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("SelectCharacter %d"), CharacterNum));

	SelectedCharacterImage->SetBrushFromMaterial(CharacterRenderTargetMaterialArray[CharacterNum]);
	PrevCharacterButton->SetIsEnabled(true);
	PrevCharacterButton = CharacterButtonArray[CharacterNum];
	PrevCharacterButton->SetIsEnabled(false);
	OnChangeSelectedCharacter.Broadcast(CharacterNameArray[CharacterNum]);
}