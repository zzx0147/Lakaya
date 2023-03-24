#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayHealthWidget.generated.h"

class UTextBlock;
class UProgressBar;

//ü���� ǥ���ϴ� Ŭ����
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
	//DamageableCharacter�� ü�� ��������Ʈ�� ��ϵǴ� �Լ�, ü���� ����� �� ȣ��
	void OnChangeHealth(AActor* Character, const float& NewHealth);
	//DamageableCharacter�� �ִ� ü�� ��������Ʈ�� ��ϵǴ� �Լ�, �ִ� ü���� ����� �� ȣ��
	void OnChangeMaximumHealth(AActor* Character, const float& NewMaximumHealth);
	//ü�� ���α׷��� �ٸ� ������Ʈ�ϴ� �Լ�
	void UpdateHealthProgressBar();

private:
	UProgressBar* HealthProgressBar;//ü���� ǥ���ϴ� ���α׷��� ��
	UTextBlock* HealthText;//ü���� ǥ���ϴ� �ؽ�Ʈ
	UTextBlock* MaximumHealthText;//�ִ� ü���� ǥ���ϴ� �ؽ�Ʈ

	float MaximumHealth;//�ִ� ü�� ��
	float Health;//ü�� ��
};
