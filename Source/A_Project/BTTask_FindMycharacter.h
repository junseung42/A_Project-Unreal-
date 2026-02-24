// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindMycharacter.generated.h"

/**
 * 
 */
UCLASS()
class A_PROJECT_API UBTTask_FindMycharacter : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindMycharacter();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
