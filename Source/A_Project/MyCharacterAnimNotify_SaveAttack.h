// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyCharacterAnimNotify_SaveAttack.generated.h"

UCLASS()
class A_PROJECT_API UMyCharacterAnimNotify_SaveAttack : public UAnimNotify
{
	GENERATED_BODY()

public:
    // 노티파이 실행시 작동 & 콤보 입력창 오픈 & 공격 판정 On
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
