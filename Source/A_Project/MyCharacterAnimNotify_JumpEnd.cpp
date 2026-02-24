// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterAnimNotify_JumpEnd.h"
#include "MyCharacter.h"

// 노티파이 실행시 작동 & 점프 Recovery끝날쯤에 작동 & bLockJump = true & 움직임 고정 풀기
void UMyCharacterAnimNotify_JumpEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (AMyCharacter* AI = MeshComp ? Cast<AMyCharacter>(MeshComp->GetOwner()) : nullptr)
    {
        // 점프 가능으로 변환
        AI->LockJump_On();

        // 움직임 고정 풀기
        AI->MoveLock_Off();
    }
}

