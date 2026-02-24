// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_Attackable.h"
#include "AIController.h"					  // OwnerComp.GetAIOwner() 사용
#include "MyCharacter.h"					  // Target을 AMyCharacter
#include "BehaviorTree/BlackboardComponent.h" // Blackboard 값 읽기 위해

// 생성자
#pragma region Constructor
UBTDecorator_Attackable::UBTDecorator_Attackable()
{
	// 노드 이름
	NodeName = TEXT("Attackable");
}
#pragma endregion

// Decorator 질문
#pragma region CalculateRawConditionValue
bool UBTDecorator_Attackable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool Result = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// Enemy 얻기
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn == nullptr)
	{
		return false;
	}

	// Blackboard의 Target키를 가져옴 -> MyCharacter인지 확인
	auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName("Target")));
	if (Target == nullptr)
	{
		return false;
	}

	// Target과 AI 사이 거리 (2.2m보다 멀다면)
	if (Target->GetDistanceTo(Pawn) > 220.f)
	{
		return false;
	}

	// 모든 조건을 통과하면 참을 반환
	return Result;
}
#pragma endregion