// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterAnimNotify_SaveAttack.h"
#include "MyCharacterAnimInstance.h"

// 노티파이 실행시 작동 & 콤보 입력창 오픈 & 공격 판정 On
void UMyCharacterAnimNotify_SaveAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (auto* AI = MeshComp ? Cast<UMyCharacterAnimInstance>(MeshComp->GetAnimInstance()) : nullptr)
    {
        AI->OpenComboWindow();     // 콤보 입력창 오픈

        AI->AnimNotify_StartHit(); // 공격 판정 On
    }
}