// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"					  // UE 핵심 타입/매크로 제공
#include "BehaviorTree/BTService.h"			  // BT에서 일정 주기로 실행되는 노드
#include "BTService_SearchTarget.generated.h" // UCLASS/리플렉션 코드 포함용

/**
 * 
 */
UCLASS()
class A_PROJECT_API UBTService_SearchTarget : public UBTService
{
	GENERATED_BODY()


public:
	bool bHadTarget = false;

// 생성자
#pragma region Constructor
public:
	UBTService_SearchTarget();
#pragma endregion

// 틱 노드
#pragma region TickNode
public:
	// OwnerComp : 이 서비스를 실행 중인 BehaviorTreeComponent(AIController가 소유)
	// NodeMemory : 이 서비스에 할당된 메모리(고급 기능; 지금은 안 씀)
	// DeltaSeconds : 지난 프레임 시간(초)
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
#pragma endregion
};
