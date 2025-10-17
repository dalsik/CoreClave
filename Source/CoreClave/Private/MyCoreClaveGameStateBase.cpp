#include "MyCoreClaveGameStateBase.h"
#include "Net/UnrealNetwork.h" // DOREPLIFETIME 매크로를 사용하기 위해 반드시 포함해야 합니다!

void AMyCoreClaveGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    // 부모 클래스의 함수를 먼저 호출해주는 것이 안전합니다.
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 여기에 복제(Replicated)할 변수들을 등록합니다.
    DOREPLIFETIME(AMyCoreClaveGameStateBase, Player1_Score);
    DOREPLIFETIME(AMyCoreClaveGameStateBase, Player2_Score);
    DOREPLIFETIME(AMyCoreClaveGameStateBase, CurrentGameState);
}

void AMyCoreClaveGameStateBase::OnRep_OnRoundTimeChanged()
{

}