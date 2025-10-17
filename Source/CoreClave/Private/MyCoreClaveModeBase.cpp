// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCoreClaveModeBase.h" // 헤더 파일 포함
#include "GameFramework/PlayerController.h" // APlayerController를 사용하기 위해 필요

// 플레이어가 로그인하면 호출됩니다.
void AMyCoreClaveModeBase::PostLogin(APlayerController* NewPlayer)
{
    // 부모 클래스의 PostLogin을 먼저 호출하는 것이 안전합니다.
    Super::PostLogin(NewPlayer);

    // 여기에 2명이 다 들어왔는지 체크하는 로직을 추가하면 됩니다.
    // 예: UE_LOG(LogTemp, Warning, TEXT("A new player has logged in!"));
}

// 라운드 시작을 알리는 함수
void AMyCoreClaveModeBase::StartRound()
{
    // 라운드 시작 로직 구현
    // 예: UE_LOG(LogTemp, Warning, TEXT("Round Started!"));
}

// 라운드 종료를 알리는 함수
void AMyCoreClaveModeBase::EndRound()
{
    // 라운드 종료 로직 구현
    // 예: UE_LOG(LogTemp, Warning, TEXT("Round Ended!"));
}