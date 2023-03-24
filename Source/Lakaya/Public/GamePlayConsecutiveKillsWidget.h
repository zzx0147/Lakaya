#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayConsecutiveKillsWidget.generated.h"

class UProgressBar;

UCLASS()
class LAKAYA_API UGamePlayConsecutiveKillsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayConsecutiveKillsWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	//���� óġ�� ����� �� ȣ��Ǵ� �Լ�(������ �ȵǾ�����)
	void OnChangeConsecutiveKills(int NewConsecutiveKills);

private:
	UProgressBar* ConsecutiveKillsProgressBar;//����óġ�� ǥ���ϴ� ���α׷��� ��
	int16 ConsecutiveKills;//����óġ Ƚ��
	int16 MaximumConsecutiveKills;//����óġ �ִ� Ƚ�� 
};
