#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Ability/Attribute/LakayaAttributeSet.h"
#include "SkillProgressBar.generated.h"

UENUM()
enum class ESkillProgressBarState : uint8
{
	None,
	CoolTime,
	StackingRegen,
	Ultimate
};

UENUM(BlueprintType)
enum class ESkillProgressBarType : uint8
{
	None,
	CoolTime,
	StackingRegen,
	Ultimate
};

UENUM(BlueprintType)
enum class ESkillKey : uint8
{
	None,
	Q,
	E,
	RMB
};


UCLASS()
class LAKAYA_API USkillProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit USkillProgressBar(const FObjectInitializer& ObjectInitializer);

	void OnEnableTimeChange(const float& ArgEnableTime);

	void SetTexture(UTexture2D* NewBackgroundImageTexture, UTexture2D* NewFillImageTexture);
	void UpdateWidget();
	FORCEINLINE void SetMaxCoolTime(const float& NewMaxCoolTime) { MaxCoolTime = NewMaxCoolTime; }

	void StartCoolTime(const float& ArgStartTime, const float& Duration);
	void StartStackingRegen(const float& ArgStartTime, const float& Duration, const bool& ArgShowProgressOnlyZeroStack = false);

	void OnChangeUltimateGaugeAttribute(const FOnAttributeChangeData& NewValue);
	void OnChangeMaxUltimateGaugeAttribute(const FOnAttributeChangeData& NewValue);
	
	void OnChangeSkillStackAttribute(const FOnAttributeChangeData& NewValue);
	void OnChangeMaxSkillStackAttribute(const FOnAttributeChangeData& NewValue);
	void SetMaxSkillStack(const float& NewValue);
	void SetSkillStack(const float& NewValue);
	
	void SetProgressType(const ESkillProgressBarType& NewType);
	FORCEINLINE ESkillProgressBarType GetProgressType() const { return ProgressBarType; }
	void SetKey(const ESkillKey& NewKey);

	FORCEINLINE	FGameplayTag GetTag() const { return SkillTag; }
	FORCEINLINE void SetTag(const FGameplayTag& NewTag) { SkillTag = NewTag; }
	
protected:
	UFUNCTION(BlueprintCallable)
	virtual void NativePreConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void CreateSkillStackImages(const uint8& Count);

	void UpdateSkillStackImages();

protected:
	UPROPERTY(EditAnywhere, Category = Skill)
	TObjectPtr<UTexture2D> BackgroundImageTexture;

	UPROPERTY(EditAnywhere, Category = Skill)
	TObjectPtr<UTexture2D> FillImageTexture;

	UPROPERTY(EditAnywhere, Category = Skill)
	TObjectPtr<UTexture2D> SkillStackFillTexture;

	UPROPERTY(EditAnywhere, Category = Skill)
	TObjectPtr<UTexture2D> SKillStackEmptyTexture;

	UPROPERTY(EditAnywhere, Category = Skill)
	TMap<ESkillKey, TObjectPtr<UTexture2D>> SkillKeyImageTextureMap;

	UPROPERTY(EditAnywhere, Category = Skill)
	FGameplayTag SkillTag;

	UPROPERTY(EditAnywhere, Category = Skill)
	float Percent;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> SkillStackPanel;

	UPROPERTY(meta = (bindwidget))
	TObjectPtr<class UProgressBar> SkillProgressBar;

	UPROPERTY(meta = (bindwidget))
	TObjectPtr<class UImage> SkillKeyImage;

	UPROPERTY()
	TArray<class UImage*> SkillStackImages;

private:
	float EnableTime;

	float StartTime;

	float MaxCoolTime;

	bool bUpdateProgressBar;

	ESkillProgressBarState CurrentState;

	uint8 bShowProgressOnlyZeroStack : 1;

	float UltimateGauge;
	float MaxUltimateGauge;

	uint8 SkillStack;
	uint8 MaxSkillStack;

	ESkillProgressBarType ProgressBarType;
};
