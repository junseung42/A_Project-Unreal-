// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"					  // UE 기본 타입/매크로 제공
#include "BehaviorTree/BTDecorator.h"		  // BT Decorator의 부모 클래스이걸 상속해야 조건 노드가 됨
#include "BTDecorator_Attackable.generated.h" // Unreal Header Tool(UHT)가 생성하는 코드

/**
 * 
 */
UCLASS()
class A_PROJECT_API UBTDecorator_Attackable : public UBTDecorator
{
	GENERATED_BODY()
// 생성자
#pragma region Constructor
public:
	UBTDecorator_Attackable();
#pragma endregion

// Decorator 질문
#pragma region CalculateRawConditionValue
public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
#pragma endregion
};
