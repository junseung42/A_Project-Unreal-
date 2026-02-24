// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyCharacterAnimNotify_ResetCombo.generated.h"

UCLASS()
class A_PROJECT_API UMyCharacterAnimNotify_ResetCombo : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 노티파이 실행시 작동 & 콤보 입력창 닫기 &  예약 입력 있으면 다음콤보 실행 & 공격 판정 Off
    virtual void Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
