#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameLobbyWeaponSelectWidget.generated.h"
 
class UImage;
class UButton;

//������ ���Ⱑ ����Ǿ��� �� �۵��ϴ� ��������Ʈ, ����� ������ ���� �ѹ��� ����� ���� �̸��� ��ȯ�մϴ�
//param1: ���� �ѹ�, param2 ���� �̸�
DECLARE_MULTICAST_DELEGATE_TwoParams(OnChangeSelectedWeaponDelegate,int32,FName)

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
	//���� ��ư�� ������ �� ȣ��Ǵ� �Լ��� �� �Լ����� �� ��ư�� ���� �Ǿ������� ���� ��ư�� �ѹ��� OnPressDraggableWeaponButton(int)�� �ѱ��
	UFUNCTION()
	void OnPressedDraggableWeapon1Button();
	UFUNCTION()
	void OnPressedDraggableWeapon2Button();
	UFUNCTION()
	void OnPressedDraggableWeapon3Button();
	UFUNCTION()
	void OnPressedDraggableWeapon4Button();

	void OnPressDraggableWeaponButton(int WeaponNum);

	//���� ��ư�� ������ ���� �� ȣ��Ǵ� �Լ��� �� �Լ����� �� ��ư�� ���� �Ǿ������� ���� ��ư�� �ѹ��� OnReleasedDraggableWeaponButton(int)�� �ѱ��
	UFUNCTION()
	void OnReleasedDraggableWeapon1Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon2Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon3Button();
	UFUNCTION()
	void OnReleasedDraggableWeapon4Button();

	void OnReleasedDraggableWeaponButton(int WeaponNum);

	//AbsoluteLocation ȭ���� Ư�� ��ġ�� ���� ��ǥ
	//ȭ���� Ư�� ��ġ�� �����ϴ� ���� ������ �ѹ��� ��ȯ�ϴ� �Լ�, ���� ������ �������� ���� ��� -1�� ��ȯ, WeaponSlotArray�� ����� ���� ���Ը� Ž���մϴ�
	int32 GetWeaponSlotAtLocation(FVector2D AbsoluteLocation);

public:
	//������ ���Ⱑ ����Ǿ��� �� �۵��ϴ� ��������Ʈ
	OnChangeSelectedWeaponDelegate OnChangeSelectedWeapon;

protected:
	bool isDragOn = false;//�巡���������� ǥ���ϴ� �÷���, ��ư�� ������ true ������Ǹ� false
	UImage* DragImage;//�巡���߿� ���콺�� ����ٴϴ� �������� �̹���

	FVector2D CurrentMousePositionWhileDrag;//�巡���� ������Ʈ�Ǵ� ���콺 ������

	TArray<FName> WeaponNameArray;//���� �̸��� �����ϴ� �迭
	TArray<UImage*> WeaponSlotArray;//���� ���� �迭, �� �迭�� ����� ���� ���Ը��� GetWeaponSlotAtLocation���� Ž���˴ϴ�
	TArray<UButton*> DraggableWeaponButtonArray;//�巡�� ������ ���� ��ư �迭
	TArray<UTexture2D*> WeaponIconTextureArray;//���� ��ư�� ���Կ� ���Ǵ� ��ư ������ �ؽ�ó
	TArray<int32> SelectedWeaponArray;//���� ���Կ� ����� ���õ� ������� �����ϴ� �迭
};