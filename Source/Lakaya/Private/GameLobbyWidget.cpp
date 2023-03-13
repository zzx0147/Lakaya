// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLobbyWidget.h"

UGameLobbyWidget::UGameLobbyWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UGameLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SelectedCharacterImage = Cast<UImage>(GetWidgetFromName(TEXT("SelectedCharacter_Img")));
	CitizenButton = Cast<UButton>(GetWidgetFromName(TEXT("Citizen_Btn")));
	GovernmentManButton = Cast<UButton>(GetWidgetFromName(TEXT("GovernmentMan_Btn")));
	GangsterButton = Cast<UButton>(GetWidgetFromName(TEXT("Gangster_Btn")));

	check(SelectedCharacterImage != nullptr);
	check(CitizenButton != nullptr);
	check(GovernmentManButton != nullptr);
	check(GangsterButton != nullptr);

	CitizenButton->OnClicked.AddDynamic(this, &UGameLobbyWidget::OnClickedCitizenButton);
	GovernmentManButton->OnClicked.AddDynamic(this, &UGameLobbyWidget::OnClickedGovernmentManButton);
	GangsterButton->OnClicked.AddDynamic(this, &UGameLobbyWidget::OnClickedGangsterButton);
}

void UGameLobbyWidget::ChangeSelectedCharacterImage(FString ImagePath)
{
	if (SelectedCharacterImage != nullptr)
	{
		
		UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, *ImagePath);
	}
}

void UGameLobbyWidget::OnClickedCitizenButton()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedCitizenButton")));
	UMaterialInterface* newMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Characters/RenderTarget/MI_Citizen"));
	SelectedCharacterImage->SetBrushFromMaterial(newMaterial);

}

void UGameLobbyWidget::OnClickedGovernmentManButton()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedGovernmentManButton")));
	UMaterialInterface* newMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_GovernmentMan"));
	SelectedCharacterImage->SetBrushFromMaterial(newMaterial);
}

void UGameLobbyWidget::OnClickedGangsterButton()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedGangsterButton")));
	UMaterialInterface* newMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_Gangster"));
	SelectedCharacterImage->SetBrushFromMaterial(newMaterial);
}

