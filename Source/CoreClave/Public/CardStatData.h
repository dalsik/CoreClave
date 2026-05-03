// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CardStatData.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FCardStatData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 기획 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CardId; // 데이터 연결용
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CardName; // 겉보기용

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ATK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost;

	// 스켈레탈 메시와 카드 아이콘 UI를 소프트 레퍼런스로 저장
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> CardMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> CardIcon;
};
