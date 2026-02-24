// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindMycharacter.h"
#include "EnemyAIController.h"                // AIController 타입 캐스팅
#include "BehaviorTree/BlackboardComponent.h" // Blackboard에 값을 쓰는 API
#include "MyCharacter.h"

UBTTask_FindMycharacter::UBTTask_FindMycharacter()
{
	NodeName = TEXT("FindMycharacter");
}

EBTNodeResult::Type UBTTask_FindMycharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI의 몸체(적 캐릭터)
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn != nullptr)
	{
		// blackboard에서 Target가져오기
		UObject* TargetObj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target"));
		if (TargetObj != nullptr)
		{
			// Mycharacter의 좌표 저장 변수
			AMyCharacter* MyCharacter = Cast<AMyCharacter>(TargetObj);

			if (MyCharacter != nullptr)
			{
				UE_LOG(LogTemp, Log, TEXT("Find MyCharacter"));

				// Blackboard에 Destination 저장
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("Destination"), MyCharacter->GetActorLocation());
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
