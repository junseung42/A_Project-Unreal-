// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterAnimNotify_JumpStart.h"
#include "MyCharacter.h"

// 노티파이 실행시 작동 & 점프 시작시 작동 & bLockJump = false
void UMyCharacterAnimNotify_JumpStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (AMyCharacter* AI = MeshComp ? Cast<AMyCharacter>(MeshComp->GetOwner()) : nullptr)
    {
        AI->LockJump_Off();
    }
}