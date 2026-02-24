// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"			 // UE 기본 타입, 매크로, UObject 시스템 포함
#include "BehaviorTree/BTTaskNode.h" // Behavior Tree의 Task 노드 부모 클래스 이걸 상속해야 실제 행동 가능
#include "BTTask_Attack.generated.h" // UHT가 자동 생성하는 코드

/**
 * 
 */
UCLASS()
class A_PROJECT_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

// 생성자
#pragma region Constructor
public:
	UBTTask_Attack();
#pragma endregion

// Task실행시 1번
#pragma region ExecuteTask
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#pragma endregion

#pragma region Tick
public:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
#pragma endregion
};
