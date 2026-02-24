// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardData.h" // BB 쓰기
#include "BehaviorTree/BehaviorTree.h"	 // BT 쓰기

// 생성자
#pragma region Constructor
AEnemyAIController::AEnemyAIController()
{
	// BT 로드
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_Enemy.BT_Enemy'"));
	if (BT.Succeeded())
	{
		BehaviorTree = BT.Object;
	}

	// BB 로드
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB(TEXT("/Script/AIModule.BlackboardData'/Game/AI/BB_Enemy.BB_Enemy'"));
	if (BB.Succeeded())
	{
		BlackboardData = BB.Object;
	}
}
#pragma endregion

// 소유/미소유
#pragma region OnPossess/OnUnPossess
// AIController가 Pawn을 소유하는 순간 호출
void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// BehaviorTree 에셋을 실행
	if (RunBehaviorTree(BehaviorTree))
	{
		UE_LOG(LogTemp, Log, TEXT("Run Behavior Tree"));
	}
}

// 소유를 풀 때 호출
void AEnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

}
#pragma endregion