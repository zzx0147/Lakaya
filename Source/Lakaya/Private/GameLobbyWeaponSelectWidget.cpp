#include "GameLobbyWeaponSelectWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"

UGameLobbyWeaponSelectWidget::UGameLobbyWeaponSelectWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UGameLobbyWeaponSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
#pragma region InitAndNullCheck

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

	WeaponNameArray = { TEXT("PiratePistol"), TEXT("Rifle"), TEXT("Pistol"), TEXT("SMG") };

	WeaponIconTextureArray =
	{
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/GunImage/T_Gun1")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/GunImage/T_Gun2")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/GunImage/T_Gun3")),
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/GunImage/T_Gun4"))
	};

	SelectedWeaponArray = { 0, 1 };

	for (auto temp : WeaponSlotArray) {check(temp != nullptr) }
	for (auto temp : DraggableWeaponButtonArray) { check(temp != nullptr) }
	for (auto temp : WeaponIconTextureArray) { check(temp != nullptr) }
	check(DragImage != nullptr);

#pragma endregion
	
	DraggableWeaponButtonArray[0]->OnPressed.AddDynamic(this, &UGameLobbyWeaponSelectWidget::OnPressedDraggableWeapon1Button);
	DraggableWeaponButtonArray[1]->OnPressed.AddDynamic(this, &UGameLobbyWeaponSelectWidget::OnPressedDraggableWeapon2Button);
	DraggableWeaponButtonArray[2]->OnPressed.AddDynamic(this, &UGameLobbyWeaponSelectWidget::OnPressedDraggableWeapon3Button);
	DraggableWeaponButtonArray[3]->OnPressed.AddDynamic(this, &UGameLobbyWeaponSelectWidget::OnPressedDraggableWeapon4Button);

	DraggableWeaponButtonArray[0]->OnReleased.AddDynamic(this, &UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeapon1Button);
	DraggableWeaponButtonArray[1]->OnReleased.AddDynamic(this, &UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeapon2Button);
	DraggableWeaponButtonArray[2]->OnReleased.AddDynamic(this, &UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeapon3Button);
	DraggableWeaponButtonArray[3]->OnReleased.AddDynamic(this, &UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeapon4Button);

}

void UGameLobbyWeaponSelectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);

	if (isDragOn)//드래그가 켜진 동안 마우스의 위치를 계속 가져와 CurrentMousePositionWhileDrag를 업데이트하고 DragImage를 마우스 위치로 계속 이동시킨다
	{
		FGeometry Geo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
		FVector2d MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		
		UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(DragImage);

		CanvasPanelSlot->SetPosition(MousePos);

		CurrentMousePositionWhileDrag = MousePos;
	}
}

void UGameLobbyWeaponSelectWidget::OnPressedDraggableWeapon1Button()
{
	OnPressDraggableWeaponButton(0);
}

void UGameLobbyWeaponSelectWidget::OnPressedDraggableWeapon2Button()
{
	OnPressDraggableWeaponButton(1);
}

void UGameLobbyWeaponSelectWidget::OnPressedDraggableWeapon3Button()
{
	OnPressDraggableWeaponButton(2);
}

void UGameLobbyWeaponSelectWidget::OnPressedDraggableWeapon4Button()
{
	OnPressDraggableWeaponButton(3);
}

void UGameLobbyWeaponSelectWidget::OnPressDraggableWeaponButton(int WeaponNum)
{
	//버튼이 눌리면 isDragOn을 ture로 설정하고 DragImage의 텍스처를 선택한 무기의 아이콘 텍스처로 변경한 뒤 Visible을 켠다
	isDragOn = true;
	DragImage->SetBrushFromTexture(WeaponIconTextureArray[WeaponNum]);
	DragImage->SetVisibility(ESlateVisibility::Visible);
}

void UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeapon1Button()
{
	OnReleasedDraggableWeaponButton(0);
}

void UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeapon2Button()
{
	OnReleasedDraggableWeaponButton(1);
}

void UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeapon3Button()
{
	OnReleasedDraggableWeaponButton(2);
}

void UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeapon4Button()
{
	OnReleasedDraggableWeaponButton(3);
}

void UGameLobbyWeaponSelectWidget::OnReleasedDraggableWeaponButton(int WeaponNum)
{
	//버튼이 릴리즈되면 드래그가 끝난다는 의미이므로 isDragOn을 false로 설정한다
	isDragOn = false;
	
	//GetAbsolutePosition은 좌상단 기준, 드래그 이미지의 중앙을 기준으로 드랍하기 위해 AbsoluteSize의 절반을 더함
	int32 TargetImageNum = GetWeaponSlotAtLocation(DragImage->GetCachedGeometry().GetAbsolutePosition() + DragImage->GetCachedGeometry().GetAbsoluteSize()/2);

	//DragImage의 AbsoluteLocation을 기준으로 드랍된 위치를
	if (-1 < TargetImageNum && TargetImageNum < WeaponSlotArray.Num())
	{
		WeaponSlotArray[TargetImageNum]->SetBrushFromTexture(WeaponIconTextureArray[WeaponNum]);
		DraggableWeaponButtonArray[SelectedWeaponArray[TargetImageNum]]->SetIsEnabled(true);
		DraggableWeaponButtonArray[WeaponNum]->SetIsEnabled(false);
		SelectedWeaponArray[TargetImageNum] = WeaponNum;
		OnChangeSelectedWeapon.Broadcast(TargetImageNum,WeaponNameArray[WeaponNum]);
	}
	
	DragImage->SetVisibility(ESlateVisibility::Hidden);
}


int32 UGameLobbyWeaponSelectWidget::GetWeaponSlotAtLocation(FVector2D AbsoluteLocation)
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
