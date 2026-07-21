// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

/// <summary>
/// 메인메뉴에서는 IMC를 사용할 필요가 없음 
/// 클릭만으로 이루어져있기 때문에.
/// </summary>
AMainMenuPlayerController::AMainMenuPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}



void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{
		return;
	}

	bShowMouseCursor = true;
	
	// 입력을 UI에만 전달하기 위함
	FInputModeUIOnly InputMode;

	// 마우스가 게임 화면 안에 강제로 갇히지 않도록 설정
	InputMode.SetLockMouseToViewportBehavior(
		EMouseLockMode::DoNotLock
	);


	SetInputMode(InputMode);
	
	// 위젯 클래스 기반
	if (MainMenuWidgetClass)
	{
		// 위젯 객체를 생성ㅅ
		MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		
		if (MainMenuWidget)
		{
			// 화면에 띠우기
			MainMenuWidget->AddToViewport();
			// 키보드 입력을 받을 위젯으로 설정한다.
			InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
			SetInputMode(InputMode);
		}
	}
	PlayMusic();
}

void AMainMenuPlayerController::PlayMusic()
{
	// 음악 설정 유무 확인 && 음악 컴포넌트 이미 존재하면 중복재생 X
	if (!MainMenuMusic && !IsValid(MainMenuMusicComponent))
	{
		return;
	}

	// 위치와 관계없이 들리는 음악 설정
	MainMenuMusicComponent =
		UGameplayStatics::SpawnSound2D(
			this,                       // World Context
			MainMenuMusic,        // 재생할 음악 에셋
			1.0f,                       // 음량
			1.0f,                       // 피치
			0.0f,                       // 시작 위치
			nullptr,                    // Concurrency 설정
			false,                      // 레벨 이동 후 유지하지 않음
			false                       // 자동 제거하지 않음
		);

	// 음악 생성에 성공하면 음량 서서히 올리며 재생
	if (IsValid(MainMenuMusicComponent))
	{
		MainMenuMusicComponent->FadeIn(MusicFadeInDuration, 1.0f);
	}
}

/// <summary>
/// 레벨 이동이나 컨트롤러 제거 시 호출되는 함수
/// </summary>
void AMainMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 레벨 이동이나 컨트롤러 제거 시 
	// 현재 재생중인 메뉴 음악 정리
	if (IsValid(MainMenuMusicComponent))
	{
		MainMenuMusicComponent->Stop();

		// 보관중인 AudioComponent 포인터를 nullptr로 초기화하여 참조를 비우도록한다.
		MainMenuMusicComponent = nullptr;
	}

	// 부모 클래스의 종료 처리 실행
	Super::EndPlay(EndPlayReason);
}