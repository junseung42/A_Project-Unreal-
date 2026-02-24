// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyCharacterAnimNotify_JumpEnd.generated.h"

UCLASS()
class A_PROJECT_API UMyCharacterAnimNotify_JumpEnd : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 노티파이 실행시 작동 & 점프 Recovery끝날쯤에 작동 & bLockJump = true & 움직임 고정 풀기
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
