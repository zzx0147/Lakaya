// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Team.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "UI/OccupyExpressElementWidget.h"
#include "CaptureArea.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCaptureAreaStateOnChangedSignature, ECaptureAreaState);
DECLARE_MULTICAST_DELEGATE_OneParam(FCaptureAreaTeamOnChangedSignature, ETeam);

UCLASS()
class LAKAYA_API ACaptureArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ACaptureArea();

	FORCEINLINE const ECaptureAreaState& GetCurrentCaptureAreaState() const { return CurrentCaptureAreaState; }
	FORCEINLINE void SetCurrentCaptureAreaState(const ECaptureAreaState& NewState) { CurrentCaptureAreaState = NewState; }

	FORCEINLINE const ETeam& GetCurrentCaptureAreaTeam() const { return CurrentCaptureAreaTeam; }
	FORCEINLINE void SetCurrentCaptureAreaTeam(const ETeam& NewTeam);

	FORCEINLINE const uint8& GetCaptureAreaID() const { return CaptureAreaId; }
protected:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/**
	 * @brief 다른 액터와 충돌됐을 때, 실행되는 함수입니다.
	 * @param OverlappedComp 겹친 컴포넌트를 나타내는 포인터입니다.
	 * @param OtherActor 겹친 다른 액터를 나타내는 포인터입니다.
	 * @param OtherComp 겹친 다른 컴포넌트를 나타내는 포인터입니다.
	 * @param OtherBodyIndex 겹친 다른 컴포넌트와의 관계를 설명하는 정수 인덱스입니다.
	 * @param bFromSweep 겹침 여부를 나타내는 bool값입니다.
	 * @param SweepResult 겹친 결과를 반환하는 FHitResult 구조체로부터 상수입니다.
	 */
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * @brief 점령중인 플레이어 리스트에 플레이어를 넣어주는 함수입니다.
	 * @param PlayerTeam 리스트에 추가할 플레이어의 소속 팀입니다.
	 * @param Player 리스트에 추가할 플레이어입니다.
	 */
	UFUNCTION()
	void AddToOccupyPlayerList(const ETeam& PlayerTeam, ALakayaBasePlayerState* Player);

	UFUNCTION()
	void RemoveFromOccupyPlayerList(const ETeam& PlayerTeam, ALakayaBasePlayerState* Player);

	/**
	 * @brief 점령구역에 누군가가 나가거나 들어오게 되었을 때, 점령구역에 인원수를 체크하여, 점령상태를 업데이트 해주는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태를 나타냅니다.
	 */
	UFUNCTION()
	void UpdateCaptureAreaState(const ECaptureAreaState& CaptureAreaState);
	
	/**
	 * @brief 점령구역에 Anti팀의 인원이 1명이상 이며 Pro팀의 인원이 0명일 때 호출되는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureAreaHandleAntiTeam(const ECaptureAreaState& CaptureAreaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원이 0명 이며 Pro팀의 인원이 1명이상일 때 호출되는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureAreaHandleProTeam(const ECaptureAreaState& CaptureAreaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원과 Pro팀의 인원이 서로 1명이상 일 때 호출되는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureAreaHandleOpposite(const ECaptureAreaState& CaptureAreaState);

	/**
	 * @brief 점령구역에 Anti팀의 인원과 Pro팀의 인원이 서로 0명일 때 호출되는 함수입니다.
	 * @param CaptureAreaState 현재 점령구역의 상태입니다.
	 */
	UFUNCTION()
	void CaptureAreaHandleNone(const ECaptureAreaState& CaptureAreaState);

	/**
	 * @brief 점령을 시도할 때 점령 게이지를 올려주는 함수입니다.
	 */
	UFUNCTION()
	void IncreaseCaptureProgress();

	/**
	 * @brief 점령을 실패했을 때 점령 게이지를 내려주는 함수입니다.
	 */
	UFUNCTION()
	void DecreaseCaptureProgress();

	
protected:
	UPROPERTY(VisibleAnywhere, Category = StaticMesh)
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> CaptureAreaWidgetComponent;
	
	UPROPERTY(Replicated)
	ECaptureAreaState CurrentCaptureAreaState = ECaptureAreaState::None;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	ETeam CurrentCaptureAreaTeam = ETeam::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	TObjectPtr<class UMaterialInterface> NeutralMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	TObjectPtr<class UMaterialInterface> AntiMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	TObjectPtr<class UMaterialInterface> ProMaterial;

	/**
	 * @brief 점령 구역은 총 3개가 있습니다. Anti구역(Anti팀에 가까운), Pro구역(Pro팀에 가까운), Center구역(가운데)
	 * Anti구역의 점령 아이디는 '1'입니다.
	 * Center구역의 점령 아이디는 '2'입니다.
	 * Pro구역의 점령 아이디는 '3'입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CaptureID")
	uint8 CaptureAreaId;
	
	TMap<ETeam, TArray<TObjectPtr<ALakayaBasePlayerState>>> OccupyingPlayerList;
	
	FCaptureAreaStateOnChangedSignature CaptureAreaStateOnChangedSignature;
	FCaptureAreaTeamOnChangedSignature CaptureAreaTeamOnChangedSignature;

	UPROPERTY()
	float AntiTeamCaptureProgress;

	UPROPERTY()
	float ProTeamCaptureProgress;

	UPROPERTY()
	float CaptureSpeed;

	UPROPERTY()
	TObjectPtr<UOccupyExpressElementWidget> OccupyExpressElementWidget;
	
	FTimerHandle CaptureProgressTimerHandle;
};