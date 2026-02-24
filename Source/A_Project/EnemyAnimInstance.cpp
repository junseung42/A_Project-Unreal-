// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

// 생성자
#pragma region Constructor
UEnemyAnimInstance::UEnemyAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_A1(TEXT("/Script/Engine.AnimMontage'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Animations/Swing1_Medium_Montage.Swing1_Medium_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_A2(TEXT("/Script/Engine.AnimMontage'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Animations/Swing2_Medium_Montage.Swing2_Medium_Montage'"));

	if (AM_A1.Succeeded()&& AM_A2.Succeeded())
	{
		AttackMontage_1 = AM_A1.Object;
		AttackMontage_2 = AM_A2.Object;
	}
}
#pragma endregion

#pragma region NativeBeginPlay()
void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		Enemy = Cast<AEnemy>(Pawn);
		if (IsValid(Enemy))
		{
			CharacterMovement = Enemy->GetCharacterMovement();
		}

	}
}
#pragma endregion
#pragma region NativeUpdateAnimation()
void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (IsValid(CharacterMovement))
	{
		FVector Velocity = CharacterMovement->Velocity;

		float Speed = Velocity.Size2D();

		ShouldMove = Speed > 3.0f;
	}
}
#pragma endregion

//공격 몽타지
#pragma region Attack Montage
void UEnemyAnimInstance::Enemy_PlayAttackMontage_1()
{
	if (IsValid(AttackMontage_1))
	{
		if (!Montage_IsPlaying(AttackMontage_1))
		{
			Montage_Play(AttackMontage_1);
		}
	}
}
void UEnemyAnimInstance::Enemy_PlayAttackMontage_2()
{
	if (IsValid(AttackMontage_2))
	{
		if (!Montage_IsPlaying(AttackMontage_2))
		{
			Montage_Play(AttackMontage_2);
		}
	}
}
#pragma endregion