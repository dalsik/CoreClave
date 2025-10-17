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
	// �������Ʈ���� ������ �� �ֵ��� Input Mapping Context ���� �����Ѵ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* GridInputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ClickAction;

	// ���� ���� �� ȣ��
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	
	//  C++���� ���� �ϰ� ������ �������Ʈ���� ����
	// ���콺 Ŭ���� �Ͼ�� �� �Լ��� ȣ��ǵ��� C++���� �������� ���̴�.
	UFUNCTION(BlueprintImplementableEvent, Category = "Grid Movement")
	void OnGridClick();
	
};
