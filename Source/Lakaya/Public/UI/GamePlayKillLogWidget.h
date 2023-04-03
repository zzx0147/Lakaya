#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayKillLogWidget.generated.h"

class UKillLogElement;

//플레이 UI중 킬 로그를 표기하는 클래스입니다
UCLASS()
class LAKAYA_API UGamePlayKillLogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void UpdateKillLogWidget(class ADamageableCharacter* Character);

protected:
	virtual void NativeConstruct() override;

private:
	// ADamageableCharacter::OnKillCharacterNotify 이벤트에 등록된 콜백함수
	void OnKillCharacterNotify(AController* KilledController, AActor* KilledActor, AController* Instigator,
	                           AActor* Causer);

	UKillLogElement* MakeFreshElement();

	class UVerticalBox* KillLogBox;
	TArray<UKillLogElement*> KillLogElementArray; //킬 로그를 표기하는 엘리먼트 배열
};
