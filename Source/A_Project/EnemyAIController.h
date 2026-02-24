// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"				 // UE 핵심 타입/매크로 제공
#include "AIController.h"				 // AAIController 클래스 사용
#include "EnemyAIController.generated.h" // 리플렉션(UCLASS/UPROPERTY)용 자동 생성 코드

/**
 * 
 */
UCLASS()
class A_PROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

// 생성자
#pragma region Constructor
public:
	AEnemyAIController();
#pragma endregion

// BT/BB
#pragma region BT/BB
private:
	// 실행할 BT 에셋
	UPROPERTY()
	class UBehaviorTree* BehaviorTree;
	//BT가 사용할 BB 자산
	UPROPERTY()
	class UBlackboardData* BlackboardData;
#pragma endregion

// 소유/미소유
#pragma region OnPossess/OnUnPossess
public:
	// AIController가 Pawn을 소유하는 순간 호출
	virtual void OnPossess(APawn* InPawn) override;
	// 소유를 풀 때 호출
	virtual void OnUnPossess() override;
#pragma endregion
};
