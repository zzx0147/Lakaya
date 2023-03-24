#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayPortraitWidget.generated.h"

class UImage;
class UTexture2D;

//플레이 UI 중 캐릭터 초상화를 관리하는 클래스입니다
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
	//캐릭터 넘버를 기준으로 초상화를 변경
	void SetCharacterPortrait(int32 CharacterNum);

private:
	UImage* CharacterPortraitImage;//캐릭터 초상화를 표기하는 Image 위젯
	TArray<UTexture2D*> CharacterPortraitTextureArray;//표기될 초상화 텍스처 배열
};
