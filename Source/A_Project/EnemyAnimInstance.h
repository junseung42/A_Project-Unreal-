// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"                 // UE 기본 타입/매크로(FVector, UE_LOG 등).
#include "Animation/AnimInstance.h"      // 애님 블루프린트의 C++ 부모 클래스인 UAnimInstance 선언
#include "EnemyAnimInstance.generated.h" // UCLASS/UPROPERTY 같은 리플렉션 코드를 UHT가 생성해 붙이는 헤더(항상 마지막에 포함)

/**
 * 
 */
UCLASS()
class A_PROJECT_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

// 캐릭터 상태 변수
#pragma region Character State Variables
public:
	UPROPERTY(Category = "Animation", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool ShouldMove = false;
#pragma endregion

// 캐릭터 & 무브먼트 포인터
#pragma region Character & Movement Pointers
public:
	// 애님 인스턴스가 붙어있는 캐릭터 포인터
	UPROPERTY(VisibleAnywhere)
	class AEnemy* Enemy = nullptr;
	// 캐릭터 이동 컴포넌트
	UPROPERTY(Category = "Animation", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UCharacterMovementComponent* CharacterMovement;
#pragma endregion

// 공격 몽타지
#pragma region Attack Montage
public:
	UPROPERTY(Category = "Animation", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_1;
	UPROPERTY(Category = "Animation", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_2;
#pragma endregion

// 생성자
#pragma region Constructor
public:
	// 생성자
	UEnemyAnimInstance();
#pragma endregion

// 오버라이드 함수
#pragma region Override function
public:
	// 플레이 시작 시점 1회
	virtual void NativeBeginPlay() override;
	// 매 프레임 호출
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
#pragma endregion

//공격 몽타지
#pragma region Attack Montage
public:
	// 공격 1타
	void Enemy_PlayAttackMontage_1();
	// 공격 2타 
	void Enemy_PlayAttackMontage_2();
#pragma endregion
};
