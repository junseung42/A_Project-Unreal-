// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterAnimNotify_ResetCombo.h"
#include "MyCharacterAnimInstance.h"

// 노티파이 실행시 작동 & 콤보 입력창 닫기 &  예약 입력 있으면 다음콤보 실행 & 공격 판정 Off
void UMyCharacterAnimNotify_ResetCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (auto* AI = MeshComp ? Cast<UMyCharacterAnimInstance>(MeshComp->GetAnimInstance()) : nullptr)
    {
        AI->CloseComboWindowAndBranch(); // 콤보 입력창 닫기 &  예약 입력 있으면 다음콤보 실행

        AI->AnimNotify_EndHit(); // 공격 판정 Off
    }
}