// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridType.generated.h"

/**
 * 해당 코드는 각 그리드가 가지고 있을 정보들을 나타내는 것이다.
 */

UENUM(BlueprintType)
enum class ECellState : uint8
{
    Empty       UMETA(DisplayName = "Empty"),
    Occupied    UMETA(DisplayName = "Occupied"),
    Blocked     UMETA(DisplayName = "Blocked")
};

// 셀 데이터 구조체
USTRUCT(BlueprintType)
struct FGridCellData
{
    GENERATED_BODY()
   
    UPROPERTY(BlueprintReadOnly)
    int32 Row = 0;
    
    UPROPERTY(BlueprintReadOnly)
    int32 Col = 0;
    
    UPROPERTY(BlueprintReadOnly)
    FVector WorldPosition = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    ECellState CellState = ECellState::Empty;
    
    // 점유 중인 유닛 (약참조로 순환참조 방지)
    TWeakObjectPtr<AActor> OccupyingUnit;
};