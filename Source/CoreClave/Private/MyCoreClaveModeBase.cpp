// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCoreClaveModeBase.h" // ��� ���� ����
#include "GameFramework/PlayerController.h" // APlayerController�� ����ϱ� ���� �ʿ�

// �÷��̾ �α����ϸ� ȣ��˴ϴ�.
void AMyCoreClaveModeBase::PostLogin(APlayerController* NewPlayer)
{
    // �θ� Ŭ������ PostLogin�� ���� ȣ���ϴ� ���� �����մϴ�.
    Super::PostLogin(NewPlayer);

    // ���⿡ 2���� �� ���Դ��� üũ�ϴ� ������ �߰��ϸ� �˴ϴ�.
    // ��: UE_LOG(LogTemp, Warning, TEXT("A new player has logged in!"));
}

// ���� ������ �˸��� �Լ�
void AMyCoreClaveModeBase::StartRound()
{
    // ���� ���� ���� ����
    // ��: UE_LOG(LogTemp, Warning, TEXT("Round Started!"));
}

// ���� ���Ḧ �˸��� �Լ�
void AMyCoreClaveModeBase::EndRound()
{
    // ���� ���� ���� ����
    // ��: UE_LOG(LogTemp, Warning, TEXT("Round Ended!"));
}