// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "MainMenuPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
	
	// 폰은 메인메뉴에서 없음.
	DefaultPawnClass = nullptr;

	// 별도의 HUD 액터 사용하지 않음
	HUDClass = nullptr;
}
