#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameLobbyWeaponSelectWidget.generated.h"
 
class UImage;
class UButton;

//선택한 무기가 변경되었을 때 작동하는 델리게이트, 변경된 무기의 슬롯 넘버와 변경된 무기 이름을 반환합니다
//param1: 무기 넘버, param2 무기 이름
DECLARE_MULTICAST_DELEGATE_TwoParams(OnChangeSelectedWeaponDelegate,int32,FName)

//무기 선택 UI 클래스
UCLASS()
class LAKAYA_API UGameLobbyWeaponSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGameLobbyWeaponSelectWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	//버튼에 바인딩되는 함수들은 UFUNTION을 사용해야함
	//무기 버튼이 눌렸을 때 호출되는 함수들 각 함수들은 각 버튼과 연결 되어있으며 누른 버튼의 넘버를 OnPressDraggableWeaponButton(int)로 넘긴다
	UFUNCTION()
	void OnPressedDraggableWeapon1Button();
	UFUNCTION()
	void OnPressedDraggableWeapon2Button();
	UFUNCTION()
	void OnPressedDraggableWeapon3Button();
	UFUNCTION()
	void OnPressedDraggableWeapon4Button();

	void OnPressDraggableWeaponButton(int WeaponNum);

	//무기 버튼이 릴리즈 됐을 때 호출되는 함수들 각 함수들은 각 버튼과 연결 되어있으며 떼진 버튼의 넘버를 OnReleasedDraggableWeaponButton(int)로 넘긴다
	UFUNCTION()
	void OnReleasedDraggableWeapon1Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon2Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon3Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon4Button();

	void OnReleasedDraggableWeaponButton(int WeaponNum);

	//AbsoluteLocation 화면의 특정 위치의 절대 좌표
	//화면의 특정 위치에 존재하는 무기 슬롯의 넘버를 반환하는 함수, 무기 슬롯이 존재하지 않을 경우 -1을 반환, WeaponSlotArray에 저장된 무기 슬롯만 탐색합니다
	int32 GetWeaponSlotAtLocation(FVector2D AbsoluteLocation);

public:
	//선택한 무기가 변경되었을 때 작동하는 델리게이트
	OnChangeSelectedWeaponDelegate OnChangeSelectedWeapon;

protected:
	bool isDragOn = false;//드래그중인지를 표기하는 플래그, 버튼이 눌리면 true 릴리즈되면 false
	UImage* DragImage;//드래그중에 마우스를 따라다니는 반투명한 이미지

	FVector2D CurrentMousePositionWhileDrag;//드래그중 업데이트되는 마우스 포지션

	TArray<FName> WeaponNameArray;//무기 이름을 저장하는 배열
	TArray<UImage*> WeaponSlotArray;//무기 슬롯 배열, 이 배열에 저장된 무기 슬롯만이 GetWeaponSlotAtLocation에서 탐색됩니다
	TArray<UButton*> DraggableWeaponButtonArray;//드래그 가능한 무기 버튼 배열
	TArray<UTexture2D*> WeaponIconTextureArray;//무기 버튼과 슬롯에 사용되는 버튼 아이콘 텍스처
	TArray<int32> SelectedWeaponArray;//무기 슬롯에 저장된 선택된 무기들을 저장하는 배열
};