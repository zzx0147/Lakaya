#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "GradeResultWidget.generated.h"

UCLASS()
class LAKAYA_API UGradeResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit  UGradeResultWidget(const FObjectInitializer& ObjectInitializer);

	virtual void RegisterPlayer(APlayerState* PlayerState);
protected:
	virtual void NativeConstruct() override;

public:
	TWeakObjectPtr<UImage> AntiTextBoxImage;
	TWeakObjectPtr<UImage> ProTextBoxImage;
	TWeakObjectPtr<UImage> VictoryImage;
	TWeakObjectPtr<UImage> DefeatImage;

	TWeakObjectPtr<UHorizontalBox> HoriBox;
	
	// GradeResult에서 각각의 플레이어 정보를 표시하는 엘리먼트의 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGradeResultElementWidget> ElementClass;
};