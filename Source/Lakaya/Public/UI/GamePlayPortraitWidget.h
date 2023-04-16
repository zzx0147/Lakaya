#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/CharacterBindableWidget.h"
#include "GamePlayPortraitWidget.generated.h"

class UImage;
class UTexture2D;

//플레이 UI 중 캐릭터 초상화를 관리하는 클래스입니다
UCLASS()
class LAKAYA_API UGamePlayPortraitWidget : public UCharacterBindableWidget
{
	GENERATED_BODY()
public:
	UGamePlayPortraitWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void BindCharacter(ACharacter* const& Character) override;
	virtual bool UnbindCharacter(ACharacter* const& Character) override;

private:
	TObjectPtr<UImage> CharacterPortraitImage;//캐릭터 초상화를 표기하는 Image 위젯
	TMap<FName,TObjectPtr<UTexture2D>> CharacterPortraitTextureMap;//표기될 초상화 텍스쳐 맵
};
