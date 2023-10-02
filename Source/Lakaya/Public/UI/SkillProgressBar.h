#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillProgressBar.generated.h"

UENUM()
enum class ESkillProgressBarState : uint8
{
	None,
	CoolTime,
	StackingRegen,
	Ultimate
};


UCLASS()
class LAKAYA_API USkillProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	USkillProgressBar(const FObjectInitializer& ObjectInitializer);

	void OnEnableTimeChange(const float& ArgEnableTime);

	void SetTexture(UTexture2D* NewBackgroundImageTexture, UTexture2D* NewFillImageTexture);
	void UpdateWidget();
	FORCEINLINE void SetMaxCoolTime(const float& NewMaxCoolTime) { MaxCoolTime = NewMaxCoolTime; }

	void StartCoolTime(const float& ArgStartTime, const float& Duration);
	void StartStackingRegen(const float& ArgStartTime, const float& Duration, const bool& ArgShowProgressOnlyZeroStack = false);

protected:
	UFUNCTION(BlueprintCallable)
	virtual void NativePreConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = Skill)
	TObjectPtr<UTexture2D> BackgroundImageTexture;

	UPROPERTY(EditAnywhere, Category = Skill)
	TObjectPtr<UTexture2D> FillImageTexture;

	UPROPERTY(EditAnywhere, Category = Skill)
	float Percent;
	TObjectPtr<class UProgressBar> SkillProgressBar;

private:
	float EnableTime;

	float StartTime;
	
	float MaxCoolTime;

	bool bUpdateProgressBar;

	ESkillProgressBarState CurrentState;

	uint8 bShowProgressOnlyZeroStack : 1;
};
