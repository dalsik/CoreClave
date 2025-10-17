// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AGridPlayerController::AGridPlayerController()
{
	// 마우스 커서가 보이도록 한다.
	bShowMouseCursor = true;
}

void AGridPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced input subsystem을 가져와서 우리가 만든 input mapping context를 추가하도록 한다.
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
		// ClickAction이 일어나면 OnGridClick 함수를 호출하도록 바인딩합니다.
		// OnGridClick의 실제 내용은 블루프린트에 있으므로, 클릭 시 블루프린트 이벤트가 실행됩니다.
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AGridPlayerController::OnGridClick);
	}
}