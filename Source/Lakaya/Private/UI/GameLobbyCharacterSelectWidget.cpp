#include "UI/GameLobbyCharacterSelectWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"

UGameLobbyCharacterSelectWidget::UGameLobbyCharacterSelectWidget(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

void UGameLobbyCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SelectedCharacterImage = Cast<UImage>(GetWidgetFromName(TEXT("SelectedCharacter_Img")));

	CharacterButtonArray =
	{
		Cast<UButton>(GetWidgetFromName(TEXT("Citizen_Btn"))),
		Cast<UButton>(GetWidgetFromName(TEXT("GovernmentMan_Btn"))),
		Cast<UButton>(GetWidgetFromName(TEXT("Gangster_Btn")))
	};

	CharacterNameArray = { TEXT("Citizen"), TEXT("GovernmentMan"), TEXT("Gangster") };

	CharacterRenderTargetMaterialArray =
	{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_Citizen")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_GovernmentMan")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_Gangster"))
	};

	check(SelectedCharacterImage != nullptr);
	check(CharacterButtonArray[0] != nullptr);
	check(CharacterButtonArray[1] != nullptr);
	check(CharacterButtonArray[2] != nullptr);

	check(CharacterRenderTargetMaterialArray[0] != nullptr);
	check(CharacterRenderTargetMaterialArray[1] != nullptr);
	check(CharacterRenderTargetMaterialArray[2] != nullptr);

	CharacterButtonArray[0]->OnClicked.AddDynamic(this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter1Button);
	CharacterButtonArray[1]->OnClicked.AddDynamic(this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter2Button);
	CharacterButtonArray[2]->OnClicked.AddDynamic(this, &UGameLobbyCharacterSelectWidget::OnClickedCharacter3Button);

	PrevCharacterButton = CharacterButtonArray[0];
	OnClickedCharacter1Button();

}

void UGameLobbyCharacterSelectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}



void UGameLobbyCharacterSelectWidget::ChangeSelectedCharacterImage(FString ImagePath)
{
	if (SelectedCharacterImage != nullptr)
	{
		UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, *ImagePath);
	}
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

void UGameLobbyCharacterSelectWidget::SelectCharacter(int32 CharacterNum)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("SelectCharacter %d"), CharacterNum));

	SelectedCharacterImage->SetBrushFromMaterial(CharacterRenderTargetMaterialArray[CharacterNum]);
	PrevCharacterButton->SetIsEnabled(true);
	PrevCharacterButton = CharacterButtonArray[CharacterNum];
	PrevCharacterButton->SetIsEnabled(false);
	OnChangeSelectedCharacter.Broadcast(CharacterNameArray[CharacterNum]);
}