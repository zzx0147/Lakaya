#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayPortraitWidget.generated.h"

class UImage;
class UTexture2D;

//�÷��� UI �� ĳ���� �ʻ�ȭ�� �����ϴ� Ŭ�����Դϴ�
UCLASS()
class LAKAYA_API UGamePlayPortraitWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	//ĳ���� �ѹ��� �������� �ʻ�ȭ�� ����
	void SetCharacterPortrait(int32 CharacterNum);

private:
	UImage* CharacterPortraitImage;//ĳ���� �ʻ�ȭ�� ǥ���ϴ� Image ����
	TArray<UTexture2D*> CharacterPortraitTextureArray;//ǥ��� �ʻ�ȭ �ؽ�ó �迭
};
