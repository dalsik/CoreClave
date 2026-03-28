// Fill out your copyright notice in the Description page of Project Settings.


#include "CardUnitActor.h"
#include "Engine/AssetManager.h" // 비동기 로딩을 위해 필수
#include "Engine/StreamableManager.h" // 비동기 로딩 관리자
#include "CardStatData.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACardUnitActor::ACardUnitActor()
{
	// 기본 컴포넌트 생성(몸체 만들기)
	MainMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainMesh"));
	RootComponent = MainMesh;

	bReplicates = true;
	SetReplicateMovement(true); // 해당 코드가 있어야 서버에서 위치를 바꾸었을 때 클라이언트에 위치 동기화 가능함
}

void ACardUnitActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// RepCardID 변수를 리플리케이션 목록에 등록.
	DOREPLIFETIME(ACardUnitActor, RepCardID);
	// 방향 변수를 리플리케이션 목록에 등록
	DOREPLIFETIME(ACardUnitActor, MoveDirection);
	// 자신의 셀을 레플리케이션 목록에 등록
	DOREPLIFETIME(ACardUnitActor, CurrentCell);
}

// 이 함수를 통해서 서버가 RepCardID 값을 바꾸어준다.
void ACardUnitActor::InitializeUnit(FName CardID)
{
	// 서버는 변수를 업데이트 한다.
	RepCardID = CardID;

	// 서버는 OnRep함수가 자동 실행되지 않으므로, 직접 로딩 함수를 호출해준다.
	LoadMeshFromID(RepCardID);
}

// 클라이언트가 RepCardID 변수를 수신하면 자동으로 실행되는 함수.
void ACardUnitActor::OnRep_CardID()
{
	// 클라이언트도 이제 CardID를 알았으니 메시를 로딩
	LoadMeshFromID(RepCardID);
}


// 카드ID를 기반으로 데이터테이블에서 일치하는 스탯을 생성하는 액터에 적용하는 함수
void ACardUnitActor::LoadMeshFromID(FName CardID)
{
		// 데이터 테이블이 연결되어 있는지 확인
	if (UnitDataTable == nullptr) // 만약 비어있다면
	{
		UE_LOG(LogTemp, Error, TEXT("데이터 테이블이 설정되지 않았습니다!"));
		return;
	}

	// 카드 ID(RowName) 로 데이터 찾기
	static const FString ContextString(TEXT("Unit Initialization"));
	FCardStatData* UnitData = UnitDataTable->FindRow<FCardStatData>(CardID, ContextString);

	if (UnitData)
	{
		// 수치 데이터 적용
		CurrentHP = UnitData->HP;
		CurrentSpeed = UnitData->Speed;

		// 스켈레탈 메시 비동기로 로딩 진행
		// 데이터에 해당 메시 경로가 설정되어 있다면 로딩을 시작해라
		if (!UnitData->CardMesh.IsNull()) // 비어있지 않다면
		{
			// 만약에 메모리에 로드가 되어있다면?
			if (UnitData->CardMesh.IsValid())
			{
				// 있으면 바로 해당 컴포넌트에 적용을 해라
				MainMesh->SetSkeletalMesh(UnitData->CardMesh.Get());
			}
			else // 만약 메모리에 로드가 되어 있지 않다면 
			{
				// 메모리에 없다면 비동기 로딩 요청
				FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();

				Streamable.RequestAsyncLoad(
					UnitData->CardMesh.ToSoftObjectPath(),
					FStreamableDelegate::CreateUObject(this, &ACardUnitActor::OnMeshLoaded, UnitData->CardMesh)
				);
			}
		}
	}
	else // 데이터 테이블 할당이 안된 경우
	{
		UE_LOG(LogTemp, Warning, TEXT("ID [%s]에 해당하는 데이터를 찾을 수 없습니다."), *CardID.ToString());
	}
}

void ACardUnitActor::OnMeshLoaded(TSoftObjectPtr<USkeletalMesh> LoadedMeshAsset)
{
	// 로딩된 메시를 안전하게 가져와서 적용
	if (USkeletalMesh* Mesh = LoadedMeshAsset.Get())
	{
		MainMesh->SetSkeletalMesh(Mesh);
	}
}

void ACardUnitActor::SetPreviewMode(bool bEnable)
{
	if (!MainMesh) return;

	if (bEnable)
	{
		// 레이캐스트에 안걸리도록 충돌 비활성화
		MainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		// 프리뷰 머터리얼로 교체
		for (int32 i = 0; i < PreviewMaterials.Num(); i++)
		{
			if (PreviewMaterials[i])
			{
				MainMesh->SetMaterial(i, PreviewMaterials[i]);
			}
		}
	}
	else
	{
		// 충돌 복구
		MainMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}