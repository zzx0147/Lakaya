#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Ability/Attribute/LakayaAttributeSet.h"
#include "Occupation/Team.h"
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
	
	FORCEINLINE void SetMaxCoolTime(const float& NewMaxCoolTime) { MaxCoolTime = NewMaxCoolTime; }

	void StartCoolTime(const float& ArgStartTime, const float& Duration);
	void StartStackingRegen(const float& ArgStartTime, const float& Duration, const bool& ArgShowProgressOnlyZeroStack = false);

	void OnChangeUltimateGaugeAttribute(const FOnAttributeChangeData& NewValue);
	void SetUltimateGauge(const float& NewValue);
	void OnChangeMaxUltimateGaugeAttribute(const FOnAttributeChangeData& NewValue);
	void SetMaxUltimateGauge(const float& NewValue);
	
	void OnChangeSkillStackAttribute(const FOnAttributeChangeData& NewValue);
	UFUNCTION(BlueprintNativeEvent)
	void SetSkillStack(const float& NewValue);
	void OnChangeMaxSkillStackAttribute(const FOnAttributeChangeData& NewValue);
	UFUNCTION(BlueprintNativeEvent)
	void SetMaxSkillStack(const float& NewValue);

	UFUNCTION(BlueprintImplementableEvent)
	void SetPercent(const float NewPercent);
	
	void SetProgressType(const ESkillProgressBarType& NewType);
	FORCEINLINE ESkillProgressBarType GetProgressType() const { return ProgressBarType; }

	FORCEINLINE	FGameplayTag GetTag() const { return SkillTag; }
	FORCEINLINE void SetTag(const FGameplayTag& NewTag) { SkillTag = NewTag; }

	UFUNCTION(BlueprintImplementableEvent)
	void SetTeam(const ETeam NewTeam);
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


protected:
	UPROPERTY(EditAnywhere, Category = Skill)
	FGameplayTag SkillTag;

	UPROPERTY()
	TArray<class UImage*> SkillStackImages;

	uint8 SkillStack;

	uint8 MaxSkillStack;
	
private:
	float EnableTime;

	float StartTime;

	float MaxCoolTime;

	bool bUpdateProgressBar;

	ESkillProgressBarState CurrentState;

	uint8 bShowProgressOnlyZeroStack : 1;

	float UltimateGauge;
	float MaxUltimateGauge;
	
	UPROPERTY(EditAnywhere)
	ESkillProgressBarType ProgressBarType;
};
