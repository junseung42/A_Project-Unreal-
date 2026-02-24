// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyCharacter_JumpRecoveryStart.generated.h"

UCLASS()
class A_PROJECT_API UMyCharacter_JumpRecoveryStart : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	// 노티파이 실행시 작동 & 점프 Recovery시작할 쯤에 작동 & 움직임 0으로 고정
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
