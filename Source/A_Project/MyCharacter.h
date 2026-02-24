// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"            // UE 기본 타입/매크로(UE_LOG, FVector 등)
#include "GameFramework/Character.h"// ACharacter 정의 불러오기(캡슐, 이동컴포넌트, 점프 등 기본 제공)
#include "MyCharacter.generated.h"  // UCLASS/UPROPERTY 같은 리플렉션 코드를 UHT가 생성해 붙이는 헤더(항상 마지막에 포함).


// 제자리 회전(각도 테이블)
#pragma region Rotation in place (angle table)
// 제자리 회전
// 행: 현재(F,R,B,L) = {0,1,2,3}
// 열: 목표(F,R,B,L) = {0,1,2,3}
static const int DeltaDeg[4][4] =
{
	/*from F*/ {    0,  +90, +180,  -90 },
	/*from R*/ {  -90,    0,  +90, -180 },
	/*from B*/ { -180,  -90,    0,  +90 },
	/*from L*/ {  +90, +180,  -90,    0 },
};
#pragma endregion

UCLASS()
class A_PROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

//애님 인스턴스
#pragma region Anim Instance
private:
	// 애님 인스턴스(스켈레탈 메쉬에 붙은)
	UPROPERTY(VisibleAnywhere)
	class UAnimInstance* AnimInstance = nullptr;
#pragma endregion

// 스프링 암 & 카메라 컴포넌트
#pragma region Spring Arm & Camera Components
private:
	// 카메라 붐(카메라 거리/회전 제어)
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComp = nullptr;
	// 실제 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComp = nullptr;       
#pragma endregion

// 마우스 감도
#pragma region Mouse sensitivity
private:
	// 좌우 감도
	UPROPERTY(EditAnywhere, Category = "Input|Mouse")
	float MouseSensitivityYaw = 0.285;   
	// 상하 감도
	UPROPERTY(EditAnywhere, Category = "Input|Mouse")
	float MouseSensitivityPitch = 0.15f; 
	// -Y 쓰는 것을 옵션화
	UPROPERTY(EditAnywhere, Category = "Input|Mouse")
	bool bInvertY = true;                
#pragma endregion

// 카메라 위/아래 피치 제한
#pragma region Camera up/down pitch limit
private:
	// 위로 올릴 수 있는 최대(보통 음수)
	UPROPERTY(EditAnywhere, Category = "Camera|Limits")
	float CameraPitchMin = -55.f; 
	// 아래로 내려갈 수 있는 최대(보통 양수)
	UPROPERTY(EditAnywhere, Category = "Camera|Limits")
	float CameraPitchMax = 60.f; 
#pragma endregion

// 제자리 회전 & 기본 이동
#pragma region Rotate in place & basic movement
private:
	// 제자리 회전 중인가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pivot", meta = (AllowPrivateAccess = "true"))
	bool bPivotRotating = false;		
	// 목표 Yaw
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pivot", meta = (AllowPrivateAccess = "true"))
	float PivotTargetYaw = 0.f;				  
	// 회전 속도(도/초) 360~720 권장
	UPROPERTY(EditAnywhere, Category = "Pivot")
	float PivotTurnRateDegPerSec = 1440.f;      
	// 목표 각도 도달 허용 오차
	UPROPERTY(EditAnywhere, Category = "Pivot")
	float PivotYawToleranceDeg = 1.f;     
	// 이동 속도 복원용 / 현재 기본 속도 (프로젝트 값에 맞춰두기)
	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeedDefault = 400.f;                 
	// 입력 유지 상태(회전 끝난 뒤 바로 이동할지 판단)
	FVector2D LastMoveAxis = FVector2D::ZeroVector;
	// Move 키가 현재 눌려있는지
	bool bMoveKeyHeld = false;                      
#pragma endregion

// 락온(카메라 회전)
#pragma region Lock-on (camera rotation)
private:
	// 회전 속도(도/초) 540~1080 권장
	UPROPERTY(EditAnywhere, Category = "Camera|LookOn")
	float LookOnYawSpeedDegPerSec = 720.f; 
	// 목표 각도 허용 오차
	UPROPERTY(EditAnywhere, Category = "Camera|LookOn")
	float LookOnYawToleranceDeg = 1.0f;   
	// 보간 진행 중인가?
	bool  bLookOnInProgress = false;     
	// 맞출 목표 Yaw
	float LookOnTargetYaw = 0.f;           
#pragma endregion

// 컨트롤러
#pragma region controller
private:
	// Enhanced Input의 IMC 에셋 참조
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext = nullptr;
	// 점프 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction = nullptr;                    
	// 무브 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction = nullptr;                    
	// 룩 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction = nullptr;                    
	// 룩온 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookOnAction = nullptr;				     
	// 런 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction = nullptr;				     
	// 어택 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction = nullptr;				     
#pragma endregion

// 카메라 충돌
#pragma region Camera crash
private:
	// 너무 가까우면 메쉬 숨김 임계값
	UPROPERTY(EditAnywhere, Category = "Camera|Collision")
	float CamMinLenToHide = 75.f; 
#pragma endregion

// 달리기
#pragma region running
private:
	// 달리기 속도
	UPROPERTY(EditAnywhere, Category = "Movement")
	float RunSpeed = 550.f;  

public:
	// 현재 달리는 중? (실제 상태)
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool bIsRunning = false; 

private:
	// 걷기 가속도
	UPROPERTY(EditAnywhere, Category = "Movement|Run")
	float WalkAcceleration = 1024.f; 
	// 달리기 가속도
	UPROPERTY(EditAnywhere, Category = "Movement|Run")
	float RunAcceleration = 3072.f;  

	// 부스트 가속도
	UPROPERTY(EditAnywhere, Category = "Movement|Run")
	float RunBoostAccel = 4000.f; 
	// 부스트 지속(초)
	UPROPERTY(EditAnywhere, Category = "Movement|Run")
	float RunBoostDuration = 0.15f; 

	// Shift를 누르는 중인가?
	bool  bRunKeyHeld = false; 
	// 부스트가 켜져있는가?
	bool  bRunBoostActive = false; 
	// 부스트 진행 시간 누적
	float RunBoostTime = 0.f;      
#pragma endregion

// 점프
#pragma region Jump
public:
	// 점프 상태 변수
	bool bLockJump = true; 

private:
	// 이동 잠금 상태
	bool bMovementLocked = false;

	// 복구용 백업 걷기 속도
	float SavedMaxWalkSpeed = 0.f;

	// 복구용 백업 가속도
	float SavedMaxAcceleration = 0.f;
#pragma endregion
	
// 공격
#pragma region Attack
	private:
		// 공격 중인지(캐릭터 상태)
		bool bAttacking = false;
		// 이전 프레임에서 공격 중이 었는지
		bool bPrevAttacking = false;
#pragma endregion

// 게임시작 몽타지
#pragma region StartMontageing
		UPROPERTY(VisibleAnywhere, Category = "State");
		bool bIntroLocked = false; // 게임시작 몽타지 연출 중인지
#pragma endregion

// 캐릭터 공격판정 상태 & 중복 타격방지
#pragma region Character attack judgment status & duplicate hit prevention
private:
	// 공격판정 중인지
	bool bHitting = false;
	// 중복 타격 방지
	TSet<AActor*> HitActorsThisSwing;
#pragma endregion

// 생성자
#pragma region Constructor
public:
	// 생성자
	AMyCharacter();
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

// 시점 함수
#pragma region Movement & Viewpoint Functions
public:
	// 기본 시야
	void MyCharacter_Look(const struct FInputActionValue& Value);  

	// 룩온 시야
	void MyCharacter_LookOn(const struct FInputActionValue& Value);
#pragma endregion

// 이동 & 제자리 회전 함수
#pragma region Rotation function in place
public:
	// 제자리 회전 시작: 자동회전 끄고, 이동속도 0으로 잠깐 묶어 "발 고정"
	void BeginPivotToYaw(float TargetYawDeg);

	// 제자리 회전 종료: 이동/자동회전 복구, 키가 여전히 눌려 있으면 즉시 이동
	void FinishPivot();

	// Move 누르는 "순간" : 회전 목표 계산하고 제자리 회전 시작
	void MyCharacter_OnMoveStarted(const FInputActionValue& Value);

	// Move를 "누르고 있는 동안": 회전 중이면 무시, 끝났으면 일반 이동
	void MyCharacter_OnMoveTriggered(const FInputActionValue& Value);

	// Move를 "뗀 순간": 홀드 상태 해제(회전 중이면 회전은 끝까지 계속)
	void MyCharacter_OnMoveCompleted(const FInputActionValue& Value);

	// 현재 캐릭터의 Yaw를 4방향 인덱스(0:F,1:R,2:B,3:L)로 변환
	static int FacingFromYaw(float YawDeg);

	// 입력 축을 4방향 인덱스(0:F,1:R,2:B,3:L)로 분류
	static int DesiredFacingFromAxis(const FVector2D& Axis);
#pragma endregion

// 달리기 시작 & 종료 함수
#pragma region Run Start & Stop Functions
public:
	// 달리기 판정(Shift 누름)
	void StartRun(const struct FInputActionValue& Value);

	// 달리기 판정(Shift 뗌)
	void StopRun(const struct FInputActionValue& Value);
#pragma endregion

// 점프 함수
#pragma region Jump function
public:
	// 점프 실행 & 상태 바꿈
	void MyCharacter_Jump(); 

	// 점프 상태 On
	void LockJump_On(); 

	// 점프 상태 Off
	void LockJump_Off();

	// 움직임 고정 풀기 & 재점프 방지
	void MoveLock_Off();

	// 움직임 고정 & 재점프 방지
	void MoveLock_On();
#pragma endregion

// 공격몽타지 실행 함수
#pragma region Attack Montage Execution Function
public:
	// 공격몽타지 실행
	void MyCharacter_Attack(const struct FInputActionValue& Value);
#pragma endregion

// 공격판정 On / Off 함수
#pragma region Attack judgment On / Off
	public:
		// 공격 판정 On
		void StartHit();
		// 공격 판정 Off
		void EndHit();
#pragma endregion
};
