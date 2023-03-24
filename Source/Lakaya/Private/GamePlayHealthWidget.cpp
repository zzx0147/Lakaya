#define DO_CHECK 1

#include "GamePlayHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "DamageableCharacter.h"


UGamePlayHealthWidget::UGamePlayHealthWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	Health = 100.0f;
	MaximumHealth = 100.0f;
}

void UGamePlayHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//�ʱ�ȭ �� ��üũ
#pragma region InitAndNullCheck

	HealthProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar_Prb")));
	HealthText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Health_Text")));
	MaximumHealthText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MaximumHealth_Text")));

	check(HealthText != nullptr);
	check(MaximumHealthText != nullptr);
	check(HealthProgressBar != nullptr);

#pragma endregion

	//ü�� �ٸ� �ִ�ġ�� �⺻ ����
	HealthProgressBar->SetPercent(1.0f);

	//DamageableCharacter�� ��������Ʈ�� ���
	ADamageableCharacter* DamageableCharacter = Cast<ADamageableCharacter>(GetOwningPlayer()->GetPawn());

	if (DamageableCharacter != nullptr)
	{
		DamageableCharacter->OnHealthReplicated.AddUObject(this, &UGamePlayHealthWidget::OnChangeHealth);
		DamageableCharacter->OnMaximumHealthReplicated.AddUObject(this, &UGamePlayHealthWidget::OnChangeMaximumHealth);
	}
}

void UGamePlayHealthWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	
}

void UGamePlayHealthWidget::OnChangeHealth(AActor* Character, const float& NewHealth)
{
	//������Ʈ�� ü���� �����ϰ� �Ҽ����� ���� �� �ؽ�Ʈ�� ǥ��
	Health = NewHealth;
	HealthText->SetText(FText::AsNumber(floor(Health)));
	//ü�� �� ������Ʈ
	UpdateHealthProgressBar();
}

void UGamePlayHealthWidget::OnChangeMaximumHealth(AActor* Character, const float& NewMaximumHealth)
{
	//������Ʈ�� �ִ� ü���� �����ϰ� �Ҽ����� ������ �ؽ�Ʈ�� ǥ��(�� �տ� /�� �ٿ��� ǥ��)
	MaximumHealth = NewMaximumHealth;
	MaximumHealthText->SetText(FText::FromString(FString::Printf(TEXT("/%f"), MaximumHealth)));
	//ü�¹� ������Ʈ
	UpdateHealthProgressBar();
}

void UGamePlayHealthWidget::UpdateHealthProgressBar()
{
	//ü�� �� ������Ʈ
	HealthProgressBar->SetPercent(Health / MaximumHealth);
}

