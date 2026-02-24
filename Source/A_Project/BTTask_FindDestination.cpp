// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindDestination.h"
#include "EnemyAIController.h"                // AIController 타입 캐스팅
#include "NavigationSystem.h"                 // 네비 시스템 API
#include "BehaviorTree/BlackboardComponent.h" // Blackboard에 값을 쓰는 API

// 생성자
#pragma region Constructor
UBTTask_FindDestination::UBTTask_FindDestination()
{
	// 노드 이름
	NodeName = TEXT("Find Patrol Destination");
}
#pragma endregion

// 작업 실행
#pragma region ExecuteTask
EBTNodeResult::Type UBTTask_FindDestination::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI의 몸체(적 캐릭터)
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn != nullptr)
	{
		// 현재 월드(레벨)의 네비 시스템
		auto NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (NavSystem != nullptr)
		{
			// 랜덤 위치 받을 변수
			FNavLocation RandomLocation;

			// 랜덤 목적지 뽑기
			if (NavSystem->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), 500.f, RandomLocation))
			{
				// Blackboard에 Destination 저장
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("Destination"), RandomLocation);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
#pragma endregion