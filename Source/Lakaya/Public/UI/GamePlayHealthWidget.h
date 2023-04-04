#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/OccupationGameState.h"
#include "GamePlayHealthWidget.generated.h"

class UTextBlock;
class UProgressBar;

//체력을 표기하는 클래스
UCLASS()
class LAKAYA_API UGamePlayHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayHealthWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
	//DamageableCharacter의 체력 델리게이트에 등록되는 함수, 체력이 변경될 때 호출
	void OnChangeHealth(AActor* Character, const float& NewHealth);
	//DamageableCharacter의 최대 체력 델리게이트에 등록되는 함수, 최대 체력이 변경될 때 호출
	void OnChangeMaximumHealth(AActor* Character, const float& NewMaximumHealth);
	//체력 프로그래스 바를 업데이트하는 함수
	void UpdateHealthProgressBar();

	UFUNCTION()
	void SetGamePlayHealthWidget(EOccupationGameState ChangeGameState);
	
private:
	UProgressBar* HealthProgressBar;//체력을 표기하는 프로그래스 바
	UTextBlock* HealthText;//체력을 표기하는 텍스트
	UTextBlock* MaximumHealthText;//최대 체력을 표기하는 텍스트

	float MaximumHealth;//최대 체력 값
	float Health;//체력 값
};
