// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class A_PROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

//애님 인스턴스
#pragma region Anim Instance
private:
	// 애님 인스턴스(스켈레탈 메쉬에 붙은)
	UPROPERTY(VisibleAnywhere)
	class UEnemyAnimInstance* EnemyAnimInstance = nullptr;
#pragma endregion

// 공격 상태 변수
#pragma region Attacking
private:
	// 공격 중인지?
	bool bIsAttacking = false;

	// 공격 첫타인지?
	bool bAttack_1 = false;
#pragma endregion

// 생성자
#pragma region Constructor
public:
	// 생성자
	AEnemy();
#pragma endregion

#pragma region BeginPlay()
protected:
	// 게임 시작 시 1회 호출
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Tick()
public:
	// 매 프레임 호출
	virtual void Tick(float DeltaTime) override;
#pragma endregion

// 입력 바인딩
#pragma region Input binding
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion

// 공격 몽타지 실행 함수
#pragma region Attack Montage Execution Function
public:
	void Enemy_Attack();
#pragma endregion

// 공격 몽타지 끝 실행 함수
#pragma region PlayAttackMontageEnd
public:
	UFUNCTION()
	// 공격 몽타지가 끝났으면 실행
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion

// 공격콤보 리셋(첫 타는 1번 공격모션)
#pragma region Attack Combo Reset (1 attack per first attack)
public:
	void ResetCombo() { bAttack_1 = false; }  // 다음 공격은 1타부터
#pragma endregion

// 상태반환 함수
#pragma region State change function
public:
	bool GetIsAttacking() const { return bIsAttacking; }
#pragma endregion
};
