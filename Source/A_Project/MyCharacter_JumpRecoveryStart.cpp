// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter_JumpRecoveryStart.h"
#include "MyCharacter.h"

// 노티파이 실행시 작동 & 점프 Recovery시작할 쯤에 작동 & 움직임 0으로 고정
void UMyCharacter_JumpRecoveryStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AMyCharacter* AI = MeshComp ? Cast<AMyCharacter>(MeshComp->GetOwner()) : nullptr)
	{
		// 움직임 0으로 고정
		AI->MoveLock_On();
	}
}