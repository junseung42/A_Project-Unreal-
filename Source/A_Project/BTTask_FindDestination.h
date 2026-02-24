// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"					  // UE 기본 타입/매크로 제공
#include "BehaviorTree/BTTaskNode.h"		  // UBTTaskNode 클래스를 사용하기 위한 include
#include "BTTask_FindDestination.generated.h" // UHT(언리얼 헤더 툴)가 생성한 리플렉션 코드 포함

/**
 * 
 */
UCLASS()
class A_PROJECT_API UBTTask_FindDestination : public UBTTaskNode
{
	GENERATED_BODY()

// 생성자
#pragma region Constructor
public:
	UBTTask_FindDestination();
#pragma endregion

// 작업 실행
#pragma region ExecuteTask
public:
	// BT에서 이 Task가 실행될 때 호출되는 핵심 함수
	// 
	// EBTNodeResult::Type
		// Succeeded : 성공 (다음 노드로 진행)
		// Failed : 실패 (Sequence면 전체 실패, Selector면 다음 후보로 넘어감)
		// InProgress : 비동기 진행 (TickTask 등 추가 구현 필요)
	// OwnerComp : 이 BT를 실행 중인 컴포넌트(= AIController가 가진 BT 컴포넌트)
	// NodeMemory : BT가 노드별로 제공하는 메모리 영역(고급 기능; 지금은 안 씀)
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#pragma endregion
};