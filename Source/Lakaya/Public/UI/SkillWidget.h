#pragma once

#include "CoreMinimal.h"
#include "SkillProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Occupation/Team.h"
#include "SkillWidget.generated.h"

UCLASS()
class LAKAYA_API USkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USkillProgressBar* GetSkillProgressBar(const ESkillKey& SkillKey) const;
	TArray<USkillProgressBar*> GetAllSkillProgressBar();

	void SetTeam(const ETeam& NewTeam);

private:
	// NativePreConstructor를 오버라이드한
	// 커스텀 위젯을 bindwidget으로 사용하면 에디터에서 클릭이 되지 않는 버그가 있습니다.
	UPROPERTY(meta = (BindWidget))  
	TObjectPtr<USkillProgressBar> QSkillProgressBar; //Q 스킬
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillProgressBar> ESkillProgressBar; //E 스킬
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillProgressBar> RMBSkillProgressBar; //우클릭 스킬
};