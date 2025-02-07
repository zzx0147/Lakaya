#pragma once

#include "CoreMinimal.h"
#include "SimpleObjectPool.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayKillLogWidget.generated.h"

class UKillLogElement;

//플레이 UI중 킬 로그를 표기하는 클래스입니다
UCLASS()
class LAKAYA_API UGamePlayKillLogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UGamePlayKillLogWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	// ADamageableCharacter::OnKillCharacterNotify 이벤트에 등록된 콜백함수
	void OnKillCharacterNotify(APlayerState* KilledController, APlayerState* Instigator,
							   AActor* Causer);
private:
	
	// 킬 로그 엘리먼트 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<UKillLogElement> KillLogClass;

	// 화면에 표시할 최대 킬 로그 개수를 지정합니다.
	UPROPERTY(EditAnywhere)
	uint8 MaxElementCount;

	class UVerticalBox* KillLogBox;
	TSimpleObjectPool<UKillLogElement> ElementPool;
	uint8 InitialChildCount;
	uint8 ShownElementCount;
};
