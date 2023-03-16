// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLobbyWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/GridSlot.h"

UGameLobbyWidget::UGameLobbyWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UGameLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SelectedCharacterImage = Cast<UImage>(GetWidgetFromName(TEXT("SelectedCharacter_Img")));
	
	CharacterButtonArray = 
	{ 
		Cast<UButton>(GetWidgetFromName(TEXT("Citizen_Btn"))),
		Cast<UButton>(GetWidgetFromName(TEXT("GovernmentMan_Btn"))),
		Cast<UButton>(GetWidgetFromName(TEXT("Gangster_Btn")))
	};

	WeaponSlotArray =
	{ 
		Cast<UImage>(GetWidgetFromName(TEXT("WeaponSlot1_Img"))),
		Cast<UImage>(GetWidgetFromName(TEXT("WeaponSlot2_Img")))
	};

	DraggableWeaponButtonArray =
	{ 
		Cast<UButton>(GetWidgetFromName(TEXT("DraggableWeapon1_Btn"))),
		Cast<UButton>(GetWidgetFromName(TEXT("DraggableWeapon2_Btn"))),
		Cast<UButton>(GetWidgetFromName(TEXT("DraggableWeapon3_Btn"))),
		Cast<UButton>(GetWidgetFromName(TEXT("DraggableWeapon4_Btn")))
	};

	DragImage = Cast<UImage>(GetWidgetFromName(TEXT("DragImage_Img")));

	CharacterNameArray = { TEXT("Citizen"), TEXT("GovernmentMan"), TEXT("Gangster") };
	WeaponNameArray = { TEXT("PiratePistol"), TEXT("Rifle"), TEXT("Pistol"), TEXT("SMG") };

	CharacterRenderTargetMaterialArray =
	{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_Citizen")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_GovernmentMan")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/RenderTarget/MI_Gangster"))
	};

	WeaponTextureArray =
	{
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/GunImage/T_Gun1")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/GunImage/T_Gun2")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/GunImage/T_Gun3")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/GunImage/T_Gun4"))
	};

	SelectedWeaponArray = { 0, 1 };

	check(SelectedCharacterImage != nullptr);
	check(CharacterButtonArray[0] != nullptr);
	check(CharacterButtonArray[1] != nullptr);
	check(CharacterButtonArray[2] != nullptr);
	check(WeaponSlotArray[0] != nullptr);
	check(WeaponSlotArray[1] != nullptr);
	check(DraggableWeaponButtonArray[0] != nullptr);
	check(DraggableWeaponButtonArray[1] != nullptr);
	check(DraggableWeaponButtonArray[2] != nullptr);
	check(DraggableWeaponButtonArray[3] != nullptr);
	check(DragImage != nullptr);
	check(CharacterRenderTargetMaterialArray[0] != nullptr);
	check(CharacterRenderTargetMaterialArray[1] != nullptr);
	check(CharacterRenderTargetMaterialArray[2] != nullptr);
	check(WeaponTextureArray[0] != nullptr);
	check(WeaponTextureArray[1] != nullptr);
	check(WeaponTextureArray[2] != nullptr);


	CharacterButtonArray[0]->OnClicked.AddDynamic(this, &UGameLobbyWidget::OnClickedCharacter1Button);
	CharacterButtonArray[1]->OnClicked.AddDynamic(this, &UGameLobbyWidget::OnClickedCharacter2Button);
	CharacterButtonArray[2]->OnClicked.AddDynamic(this, &UGameLobbyWidget::OnClickedCharacter3Button);
	
	DraggableWeaponButtonArray[0]->OnPressed.AddDynamic(this, &UGameLobbyWidget::OnPressedDraggableWeapon1Button);
	DraggableWeaponButtonArray[1]->OnPressed.AddDynamic(this, &UGameLobbyWidget::OnPressedDraggableWeapon2Button);
	DraggableWeaponButtonArray[2]->OnPressed.AddDynamic(this, &UGameLobbyWidget::OnPressedDraggableWeapon3Button);
	DraggableWeaponButtonArray[3]->OnPressed.AddDynamic(this, &UGameLobbyWidget::OnPressedDraggableWeapon4Button);

	DraggableWeaponButtonArray[0]->OnReleased.AddDynamic(this, &UGameLobbyWidget::OnReleasedDraggableWeapon1Button);
	DraggableWeaponButtonArray[1]->OnReleased.AddDynamic(this, &UGameLobbyWidget::OnReleasedDraggableWeapon2Button);
	DraggableWeaponButtonArray[2]->OnReleased.AddDynamic(this, &UGameLobbyWidget::OnReleasedDraggableWeapon3Button);
	DraggableWeaponButtonArray[3]->OnReleased.AddDynamic(this, &UGameLobbyWidget::OnReleasedDraggableWeapon4Button);


	PrevCharacterButton = CharacterButtonArray[0];
	OnClickedCharacter1Button();
}

void UGameLobbyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);

	if (isDragOn)
	{
		FGeometry Geo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
		FVector2d MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		
		UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(DragImage);

		CanvasPanelSlot->SetPosition(MousePos);

		CurrentMousePosisionWhileDrag = MousePos;
	}
}

void UGameLobbyWidget::ChangeSelectedCharacterImage(FString ImagePath)
{
	if (SelectedCharacterImage != nullptr)
	{
		UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, *ImagePath);
	}
}

void UGameLobbyWidget::OnClickedCharacter1Button()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedCitizenButton")));
	SelectCharacter(0);
}

void UGameLobbyWidget::OnClickedCharacter2Button()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedGovernmentManButton")));
	SelectCharacter(1);
}

void UGameLobbyWidget::OnClickedCharacter3Button()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnClickedGangsterButton")));
	SelectCharacter(2);
}

void UGameLobbyWidget::SelectCharacter(int32 CharacterNum)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("SelectCharacter %d"), CharacterNum));

	SelectedCharacterImage->SetBrushFromMaterial(CharacterRenderTargetMaterialArray[CharacterNum]);
	PrevCharacterButton->SetIsEnabled(true);
	PrevCharacterButton = CharacterButtonArray[CharacterNum];
	PrevCharacterButton->SetIsEnabled(false);
	OnChangeSelectedCharacter.Broadcast(CharacterNameArray[CharacterNum]);
}

void UGameLobbyWidget::OnPressedDraggableWeapon1Button()
{
	OnPressDraggableWeaponButton(0);
}

void UGameLobbyWidget::OnPressedDraggableWeapon2Button()
{
	OnPressDraggableWeaponButton(1);
}

void UGameLobbyWidget::OnPressedDraggableWeapon3Button()
{
	OnPressDraggableWeaponButton(2);
}

void UGameLobbyWidget::OnPressedDraggableWeapon4Button()
{
	OnPressDraggableWeaponButton(3);
}

void UGameLobbyWidget::OnPressDraggableWeaponButton(int WeaponNum)
{
	isDragOn = true;
	DragImage->SetBrushFromTexture(WeaponTextureArray[WeaponNum]);
	DragImage->SetVisibility(ESlateVisibility::Visible);
}

void UGameLobbyWidget::OnReleasedDraggableWeapon1Button()
{
	OnReleasedDraggableWeaponButton(0);
}

void UGameLobbyWidget::OnReleasedDraggableWeapon2Button()
{
	OnReleasedDraggableWeaponButton(1);
}

void UGameLobbyWidget::OnReleasedDraggableWeapon3Button()
{
	OnReleasedDraggableWeaponButton(2);
}

void UGameLobbyWidget::OnReleasedDraggableWeapon4Button()
{
	OnReleasedDraggableWeaponButton(3);
}

void UGameLobbyWidget::OnReleasedDraggableWeaponButton(int WeaponNum)
{
	isDragOn = false;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%f:::%f"),CurrentMousePosisionWhileDrag.X,CurrentMousePosisionWhileDrag.Y));
	//FVector2d vec = DragImage->GetCachedGeometry();
	//FVector2d pixelPos, ViewportPos;

	/*USlateBlueprintLibrary::AbsoluteToViewport(this, DragImage->GetCachedGeometry().GetAbsolutePosition(), pixelPos,ViewportPos);*/

	//FVector2d ImageSize = USlateBlueprintLibrary::GetAbsoluteSize(DragImage->GetCachedGeometry());
	//FVector2d ImageSize = DragImage->GetCachedGeometry().GetAbsoluteSize();

	//뷰포트 기준 정확한 좌표 구하는 코드
	//USlateBlueprintLibrary::LocalToViewport(this, DragImage->GetCachedGeometry(), FVector2d::ZeroVector, pixelPos, ViewportPos);

	/*GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f:::%f"), ViewportPos.X, ViewportPos.Y));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%f:::%f"), ImageSize.X, ImageSize.Y));*/
	int32 TargetImageNum = GetWeaponSlotAtLocation(DragImage->GetCachedGeometry().GetAbsolutePosition());

	if (-1 < TargetImageNum && TargetImageNum < WeaponSlotArray.Num())
	{
		WeaponSlotArray[TargetImageNum]->SetBrushFromTexture(WeaponTextureArray[WeaponNum]);
		DraggableWeaponButtonArray[SelectedWeaponArray[TargetImageNum]]->SetIsEnabled(true);
		DraggableWeaponButtonArray[WeaponNum]->SetIsEnabled(false);
		SelectedWeaponArray[TargetImageNum] = WeaponNum;
		OnChangeSelectedWeapon.Broadcast(TargetImageNum,WeaponNameArray[WeaponNum]);
	}
	
	DragImage->SetVisibility(ESlateVisibility::Hidden);
}


int32 UGameLobbyWidget::GetWeaponSlotAtLocation(FVector2D AbsoluteLocation)
{
	for (int i = 0; i < WeaponSlotArray.Num(); ++i)
	{
		FGeometry GeometryTemp = WeaponSlotArray[i]->GetCachedGeometry();
		FVector2d LeftTop = GeometryTemp.GetAbsolutePosition();
		FVector2D RightBottom = LeftTop + GeometryTemp.GetAbsoluteSize();
		
		if (LeftTop.X <= AbsoluteLocation.X && AbsoluteLocation.X <= RightBottom.X && LeftTop.Y <= AbsoluteLocation.Y && AbsoluteLocation.Y <= RightBottom.Y)
		{
			return i;
		}
	}

	return -1;
}
