// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SearchTarget.h"
#include "AIController.h"                     // AIController 포함
#include "MyCharacter.h"			          // MyCharacter 해더
#include "Engine/OverlapResult.h"	          // FOverlapResult 구조체 정의 포함
#include "BehaviorTree/BlackboardComponent.h" // BlackboardComponent 쓰기


// 생성자
#pragma region Constructor
UBTService_SearchTarget::UBTService_SearchTarget()
{
	NodeName = TEXT("Search Target");

	// TickNode를 얼마나 빠르게 돌릴건지 (1초)
	Interval = 0.3f;
}
#pragma endregion

// 틱 노드
#pragma region TickNode
void UBTService_SearchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Enemy를 가져옴
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn != nullptr)
	{
		// Overlap에 필요한 파라미터
		//
		//
		FVector Center = Pawn->GetActorLocation();				   // Enemy의 위치
		float SearchDistance = 750.0f;							   // 탐색 반경(750cm)
		TArray<FOverlapResult> OverlapResults;					   // Overlap에 걸린 애들담기(동적배열)
		FCollisionQueryParams QueryParams(NAME_None, false, Pawn); // 충돌/쿼리 옵션을 담는 파라미터 객체

		bool Result = GetWorld()->OverlapMultiByChannel
		(
			OverlapResults,                              // 충돌한는 애들
			Center,                                      // 중심
			FQuat::Identity,                             // 회전값
			ECC_GameTraceChannel4,                       // 채널(DetectPlayer)
			FCollisionShape::MakeSphere(SearchDistance), // 충돌 형태 만들기
			QueryParams                                  // 쿼리 옵션 전달
		);

		if (Result)
		{
			// 충돌한는 애
			for (auto& OverlapResult : OverlapResults)
			{
				// Overlap 결과에서 액터를 꺼내고 MyCharacter인지 아닌지 확인
				AMyCharacter* Player = Cast<AMyCharacter>(OverlapResult.GetActor());

				// MyCharacter이다면 실행1
				if (Player)
				{
					// 디버그
					DrawDebugSphere(GetWorld(), Center, SearchDistance, 10, FColor::Green, false, 0.5f);

					// Target이라는 Blackboard 메모리에 Player이 들어감
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Target"), Player);
					return;
				}
			}
			// 디버그 
			DrawDebugSphere(GetWorld(), Center, SearchDistance, 10, FColor::Red, false, 0.5f);

			// Target이라는 Blackboard 메모리에 nullptr이 들어감
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Target"), nullptr);
		}
		else
		{
			// 디버그
			DrawDebugSphere(GetWorld(), Center, SearchDistance, 10, FColor::Red, false, 0.5f);

			// Target이라는 Blackboard 메모리에 nullptr이 들어감
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("Target"), nullptr);
		}
	}
}
#pragma endregion