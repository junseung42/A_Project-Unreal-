// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"					   // UE 기본 타입/매크로(FVector, UE_LOG 등).
#include "Animation/AnimInstance.h"            // 애님 블루프린트의 C++ 부모 클래스인 UAnimInstance 선언
#include "MyCharacterAnimInstance.generated.h" // UCLASS/UPROPERTY 같은 리플렉션 코드를 UHT가 생성해 붙이는 헤더(항상 마지막에 포함)

UCLASS()
class A_PROJECT_API UMyCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

// 캐릭터 상태 변수
#pragma region Character State Variables
private:
	// 이동상태
	UPROPERTY(Category = "Ani", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool ShouldMove = false;      
	// 달리기 상태
	UPROPERTY(Category = "Ani", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool Running = false;        
	// 떨어지는 상태
	UPROPERTY(Category = "Ani", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool IsFalling = false;  
	// 점프 가능한 상태
	UPROPERTY(Category = "Ani", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bLockJumpAnim = false;
	//현재속도
	UPROPERTY(Category = "Ani", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector Velocity = FVector::ZeroVector; 
	// 수평속도
	UPROPERTY(Category = "Ani", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float GroundSpeed = 0.f;                
#pragma endregion

// 2D 블렌드 스페이스용 축 값
#pragma region Axis values ​​for 2D blend space
private:
	// 수평
	UPROPERTY(Category = "Ani", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Horizontal = 0.f;
	// 수직
	UPROPERTY(Category = "Ani", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Vertical = 0.f;
#pragma endregion

// 캐릭터 & 무브먼트 포인터
#pragma region Character & Movement Pointers
public:
	// 애님 인스턴스가 붙어있는 캐릭터 포인터
	UPROPERTY(VisibleAnywhere)
	class AMyCharacter* MyCharacter = nullptr;
	// 캐릭터 이동 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class UCharacterMovementComponent* CharacterMovement = nullptr; 
#pragma endregion

// 공격 몽타지 & 콤보
#pragma region Attack Montage & Combo
public:
	// 공격 몽타지_A
	UPROPERTY(VisibleAnywhere)
	UAnimMontage* AttackMontage_1;
	// 공격 몽타지_B
	UPROPERTY(VisibleAnywhere)
	UAnimMontage* AttackMontage_2;
	// 공격 몽타지_C
	UPROPERTY(VisibleAnywhere)
	UAnimMontage* AttackMontage_3;

public:
	// 0:대기, 1/2/3: 현재 진행중 단계
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	int32 ComboStep = 0;           
	// 콤보 입력 수용 가능 판별
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bComboWindowOpen = false; 
	// 창이 열렸을 때 들어온 입력 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bQueuedNext = false;
	// 창 밖 입력 저장(버퍼)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bBufferedAttack = false;
	// 버퍼 만료 시각(데드라인)(버퍼 입력이 언제까지 유효한지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	float BufferedAttackExpireTime = 0.f;
	// 버퍼유효 시간길이
	const float BufferDuration = 0.15f; //150ms 동안만 “미리 눌렀던 입력”을 인정
#pragma endregion

// 게임시작 몽타지
#pragma region GameStartMontage
public:
	// 게임시작 몽타지
	UPROPERTY(VisibleAnywhere)
	UAnimMontage* GameStartMontage;
#pragma endregion

// 생성자
#pragma region Constructor
public:
	// 생성자
	UMyCharacterAnimInstance();
#pragma endregion

// 오버라이드 함수
#pragma region Override function
	// 애님 인스턴스 생성 시점에 1회
	virtual void NativeInitializeAnimation() override;

	// 플레이 시작 시점 1회
	virtual void NativeBeginPlay() override;

	// 매 프레임 호출
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

#pragma endregion

// 공격 몽타지 & 콤보 함수
#pragma region Attack Montage & Combo Functions
public:
	// 공격 입력 처리(캐릭터가 호출)
	void PlayAttackMontage();

	// SaveAttack 노티파이에서 호출 & 콤보창 열기
	void OpenComboWindow();  

	// ResetCombo 노티파이에서 호출 & 콤보창 닫기
	void CloseComboWindowAndBranch(); 

private:
	// 공격 콤보에 따른 몽타지 실행
	void PlayStep(int32 Step); 

	// 콤보변수 초기화
	void ClearCombo();

#pragma endregion

// 게임시작 몽타지 함수
#pragma region PlayGameStartMontage
public:
	// 게임 시작 몽타지 실행
	void PlayGameStartMontage();
#pragma endregion

// 공격 몽타지 판별 함수
#pragma region Attack montage identification
public:
	// 공격 몽타지 실행 중인지 판별
	bool IsAttackMontagePlaying() const;
#pragma endregion

// 공격 판정 On/Off 함수
#pragma region Attack judgment On / Off
public:
	// 공격 판정 On
	void AnimNotify_StartHit();
	// 공격 판정 Off
	void AnimNotify_EndHit();
#pragma endregion
};
