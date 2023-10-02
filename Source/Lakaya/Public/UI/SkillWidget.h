#pragma once

#include "CoreMinimal.h"
#include "SkillProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "SkillWidget.generated.h"


USTRUCT(BlueprintType)
struct FSkillWidgetDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ESkillProgressBarType QSkillType;
	UPROPERTY(EditAnywhere)
	UTexture2D* QSkillTextureFillImage;
	UPROPERTY(EditAnywhere)
	UTexture2D* QSkillTextureBackgroundImage;
	UPROPERTY(EditAnywhere)
	FGameplayTag QSkillTag;

	UPROPERTY(EditAnywhere)
	ESkillProgressBarType ESkillType;
	UPROPERTY(EditAnywhere)
	UTexture2D* ESkillTextureFillImage;
	UPROPERTY(EditAnywhere)
	UTexture2D* ESkillTextureBackgroundImage;
	UPROPERTY(EditAnywhere)
	FGameplayTag ESkillTag;
	
	UPROPERTY(EditAnywhere)
	ESkillProgressBarType RMBSkillType;
	UPROPERTY(EditAnywhere)
	UTexture2D* RMBSkillTextureFillImage;
	UPROPERTY(EditAnywhere)
	UTexture2D* RMBSkillTextureBackgroundImage;
	UPROPERTY(EditAnywhere)
	FGameplayTag RMBSkillTag;
};

UCLASS()
class LAKAYA_API USkillWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
public:
	void SetCharacter(const FName& CharacterName);
	USkillProgressBar* GetSkillProgressBar(const ESkillKey& SkillKey) const;
	TArray<USkillProgressBar*> GetAllSkillProgressBar();
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USkillProgressBar> QSkillProgressBar;//Q 스킬
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USkillProgressBar> ESkillProgressBar;//E 스킬
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USkillProgressBar> RMBSkillProgressBar;//우클릭 스킬
	
	// UPROPERTY(EditAnywhere)
	// TMap<FName, FSkillWidgetTextureStruct> SkillWidgetTextureMap;

	UPROPERTY(EditAnywhere)
	FSkillWidgetDataStruct SkillWidgetData;
};