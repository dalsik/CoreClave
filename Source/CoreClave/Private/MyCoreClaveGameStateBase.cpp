#include "MyCoreClaveGameStateBase.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME ��ũ�θ� ����ϱ� ���� �ݵ�� �����ؾ� �մϴ�!

void AMyCoreClaveGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    // �θ� Ŭ������ �Լ��� ���� ȣ�����ִ� ���� �����մϴ�.
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // ���⿡ ����(Replicated)�� �������� ����մϴ�.
    DOREPLIFETIME(AMyCoreClaveGameStateBase, Player1_Score);
    DOREPLIFETIME(AMyCoreClaveGameStateBase, Player2_Score);
    DOREPLIFETIME(AMyCoreClaveGameStateBase, CurrentGameState);
}

void AMyCoreClaveGameStateBase::OnRep_OnRoundTimeChanged()
{

}