// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyCharacterAnimNotify_JumpStart.generated.h"

UCLASS()
class A_PROJECT_API UMyCharacterAnimNotify_JumpStart : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 노티파이 실행시 작동 & 점프 시작시 작동 & bLockJump = false
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
