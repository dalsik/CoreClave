// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UAudioComponent;
class USoundBase;
class UUserWidget;

/**
 * 
 */
UCLASS()
class CORECLAVE_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainMenuPlayerController();
	
protected:
	virtual void BeginPlay() override;

	// 레벨 이동 또는 플레이어 컨트롤러 제거 시에 호출
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> MainMenuWidget;

	// 재생할 음악
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main Menu")
	TObjectPtr<USoundBase> MainMenuMusic;
	
	// 현재 음악 제어하기 위한 오디오 컴포넌트
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> MainMenuMusicComponent;
	
	// 음악 재생 시작 시 Fade In
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Main Menu", meta = (ClampMin = "0.0"))
	float MusicFadeInDuration = 1.0f;

	void PlayMusic();
	
};
