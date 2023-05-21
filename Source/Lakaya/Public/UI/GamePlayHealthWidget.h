#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayHealthWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;

//체력을 표기하는 클래스
UCLASS()
class LAKAYA_API UGamePlayHealthWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// 위젯에 표시되는 현재 체력 수치를 업데이트합니다.
	void SetCurrentHealth(const float& NewHealth);

	// 위젯에 표시되는 전체 체력 수치를 업데이트합니다.
	void SetMaximumHealth(const float& NewMaximumHealth);

private:
	//체력 프로그래스 바를 업데이트하는 함수
	void UpdateHealthProgressBar() const;

	TObjectPtr<UProgressBar> HealthProgressBar; //체력을 표기하는 프로그래스 바
	TObjectPtr<UTextBlock> HealthText; //체력을 표기하는 텍스트
	TObjectPtr<UTextBlock> MaximumHealthText; //최대 체력을 표기하는 텍스트

	float MaximumHealth; //최대 체력 값
	float Health; //체력 값

public:
	TObjectPtr<UImage> UserInfoCharImageRena;	// 캐릭터 정보창에 표기되는 레나 초상화
	TObjectPtr<UImage> UserInfoCharImageWazi;	// 캐릭터 정보창에 표기되는 와지 초상화
};
