// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "EnemyAIController.h" 
#include "Enemy.h"

// 생성자
#pragma region Constructor
UBTTask_Attack::UBTTask_Attack()
{
	// 노드이름
	NodeName = TEXT("Attack");

	// TickTask = true
	bNotifyTick = true;
}
#pragma endregion

// Task실행시 1번
#pragma region ExecuteTask
EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// Enemy가져오기
	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	// 실패시
	if (Enemy == nullptr)
		return EBTNodeResult::Failed; // Task 실행 불가

	if (Enemy->GetIsAttacking() == false)
	{
		Enemy->Enemy_Attack();
		return EBTNodeResult::InProgress; // Task 진행중
	}

	Enemy->Enemy_Attack();

	return Result; // 의미 없음
}
#pragma endregion

#pragma region TickTask
void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Enemy가져오기
	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	// Enemy가 죽을경우/잃어버릴경우 정상적으로 끝내기 위해서 Succeeded
	if (Enemy == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (Enemy->GetIsAttacking() == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
#pragma endregion