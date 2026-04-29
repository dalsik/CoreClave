// Fill out your copyright notice in the Description page of Project Settings.
// 해당 코드는 데이터 테이블을 활용하여 자동으로 할당해주는 식으로의 프로세스를 나타내는 코드이다.
// 또한 비동기 생성을 통해서 로딩시간을 단축하며, 메모리관리를 효율적으로 활용한다.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardUnitActor.generated.h"

class AGridCell;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveCompleted);

UCLASS()
class CORECLAVE_API ACardUnitActor : public AActor
{
	GENERATED_BODY()
	
public:	
	virtual void Tick(float DeltaTime) override;
	// Sets default values for this actor's properties
	ACardUnitActor();

	// 리플리케이션을 위해 필요한 함수(CardID 등록용)
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Unit Data")
	int32 MoveDirection = 1; // +1 or -1 

	// 자신이 위치한 셀 참조
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Grid")
	AGridCell* CurrentCell = nullptr;

	// 현재 체력 등등(나중에 사용할 게임 로직용 변수)
	float CurrentHP;
	float CurrentSpeed;

protected:
	
	// 유닛의 몸체가 될 스클레탈 메시 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MainMesh;

	// 에디터에서 지정할 데이터 테이블(여기에 엑셀 데이터를 넣어야 한다)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Data")
	UDataTable* UnitDataTable;

	// 값이 바뀌는 것을 감지해서 전체 클라이언트에게 적용을 해줄 변수명 선언
	UPROPERTY(ReplicatedUsing = OnRep_CardID, BlueprintReadOnly, Category = "Unit Data")
	FName RepCardID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
	TArray<UMaterialInterface*> PreviewMaterials;

	// 클라이언트가 CardID를 받으면 자동으로 실행할 함수
	UFUNCTION()
	void OnRep_CardID();

	// 메시 로딩 로직을 따로 분리 (재사용성을 위해)
	void LoadMeshFromID(FName CardID);

public:	
	// 블루프린트에서 호출할 초기화 함수
	UFUNCTION(BlueprintCallable, Category = "Unit Data")
	void InitializeUnit(FName CardID);

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void SetPreviewMode(bool bEnable);

/// <summary>
/// 유닛이 다음 칸으로 넘어갈 때 자연스럽게 넘어가질 수 있도록 설정하기 위한 로직들
/// </summary>

	// 이동 완료 시 실행되는 콜백
	UPROPERTY(BlueprintAssignable, Category = "Movement")
	FOnMoveCompleted OnMoveCompleted;

	// 현재 이동중인지
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsMoving = false;

	// 목표 위치로 부드럽게 이동
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveToLocation(FVector InTargetLocation, float Duration = 0.3f);
	

private:
	// 비동기 로딩이 끝났을 때 실행될 콜 백 함수
	void OnMeshLoaded(TSoftObjectPtr<USkeletalMesh> LoadedMeshAsset);

	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	UPROPERTY()
	bool bIsPreviewMode = false;

	FVector StartLocation;
	FVector TargetLocation;
	float MoveElapsed = 0.0f;
	float MoveDuration = 0.3f;
};
