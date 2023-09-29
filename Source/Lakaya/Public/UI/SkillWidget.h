#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/ArmedCharacter.h"
#include "SkillWidget.generated.h"

USTRUCT(BlueprintType)
struct FSkillWidgetTextureStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	UTexture2D* QSkillTextureFillImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* QSkillTextureBackgroundImage;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* ESkillTextureFillImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* ESkillTextureBackgroundImage;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* RMBSkillTextureFillImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* RMBSkillTextureBackgroundImage;
};

UCLASS()
class LAKAYA_API USkillWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void SetCharacter(const FName& CharacterName);
	class USkillProgressBar* GetSkillProgressBar(EAbilityKind AbilityKind);
	
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USkillProgressBar> QSkillProgressBar;//Q 스킬
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USkillProgressBar> ESkillProgressBar;//E 스킬
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USkillProgressBar> RMBSkillProgressBar;//우클릭 스킬

	UPROPERTY(EditAnywhere)
	TMap<FName, FSkillWidgetTextureStruct> SkillWidgetTextureMap;
};