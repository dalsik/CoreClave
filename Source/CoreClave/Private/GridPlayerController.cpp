// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AGridPlayerController::AGridPlayerController()
{
	// ���콺 Ŀ���� ���̵��� �Ѵ�.
	bShowMouseCursor = true;
}

void AGridPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced input subsystem�� �����ͼ� �츮�� ���� input mapping context�� �߰��ϵ��� �Ѵ�.
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(GridInputMappingContext, 0);
	}
}

void AGridPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// ClickAction�� �Ͼ�� OnGridClick �Լ��� ȣ���ϵ��� ���ε��մϴ�.
		// OnGridClick�� ���� ������ �������Ʈ�� �����Ƿ�, Ŭ�� �� �������Ʈ �̺�Ʈ�� ����˴ϴ�.
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AGridPlayerController::OnGridClick);
	}
}