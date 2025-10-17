// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GridPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class CORECLAVE_API AGridPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AGridPlayerController();
protected:
	// 블루프린트에서 설정할 수 있도록 Input Mapping Context 변수 노출한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* GridInputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ClickAction;

	// 게임 시작 시 호출
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	
	//  C++에서 선언만 하고 구현은 블루프린트에서 진행
	// 마우스 클릭이 일어나면 이 함수가 호출되도록 C++에서 연결해줄 것이다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Grid Movement")
	void OnGridClick();
	
};
