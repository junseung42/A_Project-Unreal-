// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h" // 카메라 붐(거리/회전 보정)
#include "Camera/CameraComponent.h"           // 실제 카메라
#include "EnhancedInputComponent.h"           // 입력 바인딩에 쓰는 컴포넌트
#include "EnhancedInputSubsystems.h"          // IMC를 등록하는 서브시스템
#include "MyCharacterAnimInstance.h"          // 커스텀 애님 인스턴스
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"                 // 충돌경로 보이게 할려고

// 생성자
#pragma region Constructor
AMyCharacter::AMyCharacter()
{
	// TiCK함수 사용여부 / 필요하지 않으면 성능 향상을 위해 이 기능을 끌 수 있음
	PrimaryActorTick.bCanEverTick = true;

	// 에셋 경로로 메쉬 로드
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MS(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonGreystone/Characters/Heroes/Greystone/Meshes/Greystone.Greystone'"));
	if (MS.Succeeded())
	{
		//매쉬 할당
		GetMesh()->SetSkeletalMesh(MS.Object);

		// 위치 및 회전 보정 (캡슐 콜리전과 맞추기 위함)
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	// 애님 블루프린트 클래스를 찾아 메쉬에 연결 / 블루프린트 경로를 가져올때는 마지막에 _C *필수*
	static ConstructorHelpers::FClassFinder<UAnimInstance> AI(TEXT("/Script/Engine.AnimBlueprint'/Game/Animation/ABP_MyCharacter.ABP_MyCharacter_C'"));
	if (AI.Succeeded())
	{
		// 할당
		GetMesh()->SetAnimInstanceClass(AI.Class);
	}

	//BP_Mycharacter블루프린트 스프링암 & 카메라 컴포넌트 생성
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	// 스프링암 & 카메라 지정된 경로로 부착
	SpringArmComp->SetupAttachment(RootComponent); // 스프링암을 루트(캡슐)에 부착
	CameraComp->SetupAttachment(SpringArmComp);    // 카메라를 스프링암 끝에 부착

	// 기본 카메라 세팅
	SpringArmComp->TargetArmLength = 250.0f;					// 카메라 거리
	SpringArmComp->SocketOffset.Z = 150;						// 카메라 소켓 Z
	CameraComp->SetRelativeRotation(FRotator(-13.f, 0.f, 0.f)); // 카메라 Transfrom 조절

	// 카메라는 컨트롤러 회전을 따라가게
	SpringArmComp->bUsePawnControlRotation = true; // 컨트롤러 회전을 스프링 암에 반영
	CameraComp->bUsePawnControlRotation = false;   // 컨트롤러 회전을 카메라 컴포넌트에 반영 X 

	// 캐릭터는 컨트롤러(Yaw) 회전을 "안 따르게"
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 속도에 따른 카메라 동적이동
	SpringArmComp->bEnableCameraLag = true;			  // 위치 랙 사용 여부 / 카메라 탄성
	SpringArmComp->CameraLagSpeed = 1.f;			  // 카메라가 목표 위치로 수렴하는 속도 / 값이 클수록 빨리 붙음
	SpringArmComp->CameraLagMaxDistance = 30.f;		  // 최대 간격
	SpringArmComp->bUseCameraLagSubstepping = true;	  // 서브스텝으로 랙 계싼을 촘촘히 수행
	SpringArmComp->CameraLagMaxTimeStep = 1.f / 60.f; // 서브스텝 1회가 처리하는 최대 시간 간격

	// 카메라 충돌
	SpringArmComp->bDoCollisionTest = true;   // 충돌 시 자동으로 암 길이 줄임
	SpringArmComp->ProbeSize = 12.f;
	SpringArmComp->ProbeChannel = ECC_GameTraceChannel5; // 카메라충돌 전용 채널

	auto* Move = GetCharacterMovement();

	// 캐릭터 기본이동
	Move->MaxWalkSpeed = 400.f;                // 기본 걷기 속도(기존 600)
	Move->MaxAcceleration = WalkAcceleration;  // 가속
	Move->BrakingDecelerationWalking = 1024.f; // 감속
	Move->GroundFriction = 12.f;               // 지면 마찰

	// 이동 중에만 이동 방향을 향해 회전(프리룩 기본)
	Move->bOrientRotationToMovement = true;         // 이동 벡터 바라보기
	Move->RotationRate = FRotator(0.f, 870.f, 0.f); // 회전 속도(도/초)

	// 점프
	Move->JumpZVelocity = 500.f;   // 점프 힘
	Move->AirControl = 0.35f;   // 공중에서의 조향
	Move->GravityScale = 1.0f;    // 기본 중력
}
#pragma endregion

// 처음 시작
#pragma region BeginPlay()
// 게임 시작 시 1회 호출
void AMyCharacter::BeginPlay()
{
	// 부모 클래스 초기화
	Super::BeginPlay();

	// 애님 인스턴스(실행객체)를 캐싱
	AnimInstance = Cast<UMyCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AMyCharacter::BeginPlay: Failed to cast AnimInstance to UMyCharacterAnimInstance. Check your Skeletal Mesh's Anim Blueprint Class!"));
	}

	// Enhanced Input의 IMC(매핑 컨텍스트)를 로컬 플레이어 서브시스템에 등록 / 등록해야 SetupPlayerInputComponent에서 바인딩한 Action들이 실제로 동작
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// 카메라 위/아래 피치 제한
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->ViewPitchMin = CameraPitchMin; // 최소 피치
			PC->PlayerCameraManager->ViewPitchMax = CameraPitchMax; // 최대 피치
		}
	}

	// 게임시작 몽타지 실행
	if (auto* AI = Cast<UMyCharacterAnimInstance>(AnimInstance))
	{
		if (!GetCharacterMovement()->IsFalling() && bMovementLocked == false)
		{
			AI->PlayGameStartMontage();
		}
	}
}
#pragma endregion

#pragma region Tick()
// 매 프레임 호출
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 속도에 따른 카메라 동적이동
	const auto* Move = GetCharacterMovement();
	const bool bMoving =
		Move && !Move->GetCurrentAcceleration().IsNearlyZero(); // 입력으로 움직이는 중?

	const float TargetMaxDist = bMoving ? 30.f : 0.f;  // 움직일 땐 50.f만큼 최대 지연거리, 멈추면 0으로
	SpringArmComp->CameraLagMaxDistance =
		FMath::FInterpTo(SpringArmComp->CameraLagMaxDistance, TargetMaxDist, DeltaTime, 1.f);
	// 마지막 1.f : 간격이 바뀌는 속도(클수록 빨리 변함)


	// 제자리 회전 
	if (bPivotRotating)                                // 제자리 회전 모드일 때만
	{
		const FRotator Curr = GetActorRotation();      // 현재 배우 회전
		const FRotator Goal(0.f, PivotTargetYaw, 0.f); // 목표 Yaw(피치/롤은 0)

		// 일정 각속도로 회전: 각도 차이가 클수록 시간이 오래 걸림(속도는 일정)
		const FRotator Next =
			FMath::RInterpConstantTo(Curr, Goal, DeltaTime, PivotTurnRateDegPerSec);

		SetActorRotation(Next);                        // 이번 프레임 회전 적용

		// 남은 각도 절대값 계산(부호 제거). 오차 이하이면 종료
		const float Remain =
			FMath::Abs(FMath::FindDeltaAngleDegrees(Next.Yaw, PivotTargetYaw));

		if (Remain <= PivotYawToleranceDeg)            // 목표에 충분히 가까워졌다면
		{
			SetActorRotation(Goal);                    // 최종 스냅 정착
			FinishPivot();                             // 이동/자동회전 복원 + 필요 시 즉시 이동
		}
	}


	// 락온(카메라 회전)
	if (bLookOnInProgress)
	{
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			const FRotator Curr = PC->GetControlRotation();              // 현재 컨트롤러 시점 회전
			const FRotator Goal(Curr.Pitch, LookOnTargetYaw, Curr.Roll); // 목표 회전

			// 일정 각속도 보간
			const FRotator Next =
				FMath::RInterpConstantTo(Curr, Goal, DeltaTime, LookOnYawSpeedDegPerSec);
			// RInterpConstantTo → 각속도가 일정하게
			// DeltaTime → 이번 프레임 시간

			// 보간 결과 적용
			PC->SetControlRotation(Next);

			// 남은 각도 체크 → 거의 같으면 스냅 후 종료
			const float remain =
				FMath::Abs(FMath::FindDeltaAngleDegrees(Next.Yaw, LookOnTargetYaw));
			// Abs → 절대값
			// FindDeltaAngleDegrees(a, b) : a → b로 갈 때의 서명 있는 최단 각도차(−180~+180)

			// 허용오차 적용
			if (remain <= LookOnYawToleranceDeg)
			{
				PC->SetControlRotation(Goal);  // 최종 스냅
				bLookOnInProgress = false;     // 보간 끝
			}
		}
		else
		{
			bLookOnInProgress = false; // 컨트롤러 없으면 중단
		}
	}


	// 카메라 충돌(숨김)
	const float CurrLen =
		(CameraComp->GetComponentLocation() - SpringArmComp->GetComponentLocation()).Size();
	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(CurrLen < CamMinLenToHide);
	}


	// 게임시작 몽타지 연출 중인지 판단
	if (AnimInstance)
	{
		auto* AI = Cast<UMyCharacterAnimInstance>(AnimInstance);

		// AI와 GameStartMontage에 몽타지가 들어가면 실행
		if (AI && AI->GameStartMontage)
		{
			// GameStartMontage가 실행중이면 true 아니면 false
			bIntroLocked = AI->Montage_IsPlaying(AI->GameStartMontage);
		}
		else
		{
			bIntroLocked = false;
		}
	}


	// 게임시작 몽타지 종료순간 감지
	//
	// 이전 프레임 bIntroLocked상태 변수 저장
	static bool bPrevIntroLocked = false;

	// 이번 프레임에 게임시작 몽타지가 종료 되었디면 실행 
	if (bPrevIntroLocked && !bIntroLocked)
	{
		// 방금 Intro가 끝남 -> 이동값 복구
		if (auto* MoveComp = GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = WalkSpeedDefault;
			MoveComp->MaxAcceleration = WalkAcceleration;
		}
	}

	// 상태값 저장
	bPrevIntroLocked = bIntroLocked;


	// 달리기
	if (auto* MoveComp = GetCharacterMovement())
	{

		// 움직임 멈춤 
		// 공격 중이거나(=이동잠금처럼 취급) + 점프착지 + 이동잠금 + 게임시작 + 몽타지 실행중 이면 강제 정지
		if (bMovementLocked || bAttacking || bPivotRotating || bIntroLocked)
		{
			MoveComp->StopMovementImmediately();
			MoveComp->MaxWalkSpeed = 0.f;
			MoveComp->MaxAcceleration = 0.f;

			// 표시상도 달리기 아님
			bIsRunning = false;
		}
		else
		{
			const bool bHasInput = !GetLastMovementInputVector().IsNearlyZero(); // 이동 입력 유/무
			const bool bOnGround = MoveComp->IsMovingOnGround();                 // 지상 여부
			const bool bRunningNow = bRunKeyHeld && bHasInput && bOnGround && !bAttacking;      // 이번 프레임에 달리기 판단

			// 상태가 바뀐 순간에만 세팅
			if (bIsRunning != bRunningNow) //걷기 → 달리기 전환
			{
				bIsRunning = bRunningNow;

				if (bIsRunning)
				{
					MoveComp->MaxWalkSpeed = RunSpeed;           // 목표 달리기 속도
					MoveComp->MaxAcceleration = RunAcceleration; // 달리기 가속도

					bRunBoostActive = true;
					RunBoostTime = 0.f;
				}
				else
				{
					MoveComp->MaxWalkSpeed = WalkSpeedDefault;    // 목표 걷기 속도
					MoveComp->MaxAcceleration = WalkAcceleration; // 걷기 가속도

					// 부스트 종료
					bRunBoostActive = false;
					RunBoostTime = 0.f;
				}
			}

			// 부스트
			if (bRunBoostActive && bIsRunning)
			{
				RunBoostTime += DeltaTime;                 // 부스트 시간 누적
				MoveComp->MaxAcceleration = RunBoostAccel; // 부스트 가속도

				if (RunBoostTime >= RunBoostDuration)      // 부스트 끝
				{
					bRunBoostActive = false;
					MoveComp->MaxAcceleration = RunAcceleration; // 평살시 달리기 가속도로 복귀
				}
			}
		}

	}


	// 공격
	
	// 이번 프레임에 계산된 공격 판정 결과(임시값)
	bool bNowAttacking = false;

	// 공격 몽타지 실행중인지 판단
	if (auto* AI = Cast<UMyCharacterAnimInstance>(AnimInstance))
	{
		bNowAttacking = AI->IsAttackMontagePlaying();
	}

	// 상태 전이 감지 / 직전 프레임과 이번 프레임이 다르면 실행(공격 시작 & 공격 종료)
	if (bNowAttacking != bPrevAttacking)
	{
		if (bNowAttacking) // 공격 시작 / 이동 잠금 1회
		{
			if (auto* MoveComp = GetCharacterMovement())
			{
				SavedMaxWalkSpeed = MoveComp->MaxWalkSpeed;
				SavedMaxAcceleration = MoveComp->MaxAcceleration;

				MoveComp->StopMovementImmediately(); // 현재속도를 즉시 0으로
				MoveComp->MaxWalkSpeed = 0.f;
				MoveComp->MaxAcceleration = 0.f;
				ConsumeMovementInputVector(); // 이동 입력 누적치를 비움
			}
			bIsRunning = false;
			bRunKeyHeld = false;
		}
		else // 공격 종료 / 이동 복구 1회
		{
			if (auto* MoveComp = GetCharacterMovement())
			{
				// 공격 끝나면 무조건 걷기
				MoveComp->MaxWalkSpeed = WalkSpeedDefault;
				MoveComp->MaxAcceleration = WalkAcceleration;

				// 피벗 중이 아니고 이동 잠금도 아니면 자동 회전 복구
				if (!bPivotRotating && !bMovementLocked)
				{
					// 이동 방향을 바라보도록 자동으로 Yaw를 돌림
					MoveComp->bOrientRotationToMovement = true;
				}
			}
		}

		// 상태값 업데이트 / 직전 프레임 상태를 이번 프레임 값으로 갱신
		bPrevAttacking = bNowAttacking;
	}
	// 최종 상태값 업데이트
	bAttacking = bNowAttacking;

	// 공격판정
	if (bHitting)
	{
		// 충돌처리
		// 
		TArray<FHitResult> HitResults; // 충돌 결과들을 저장할 변수(동적배열)

		// 자기 자신 무시
		//
		// 충돌 검사 옵션
		FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackSweep), false, this);
		Params.AddIgnoredActor(this);

		const float Radius = 40.f;     // 공격 반지름(캡슐)
		const float Range = 150.f;     // 공격 거리
		const float HalfHeight = 90.f; // 캡슐의 높이 / 2

		const FVector StartPos = GetActorLocation();                                 // 시작 지점
		const FVector EndPos = GetActorLocation() + GetActorForwardVector() * Range; // 끝 지점

		
		const FVector Dir = (EndPos - StartPos).GetSafeNormal();    // 공격 방향벡터(길이 1)
		const FQuat Rot = FRotationMatrix::MakeFromX(Dir).ToQuat(); // x축이 Dir을 쪽으로 회전

		// 판정 여부
		bool bAnyHit = GetWorld()->SweepMultiByChannel
		(
			HitResults,									      // 충돌 결과를 저장할 변수					
			StartPos,									      // 시작 지점
			EndPos,										      // 끝 지점
			Rot,											  // 회전
			ECC_GameTraceChannel1,						      // 충돌 채널 (Attack)
			FCollisionShape::MakeCapsule(Radius, HalfHeight), // 형태 : 반지름
			Params										      // 충돌 쿼리 파라미터
		);

		// 디버그 표시
		// 
		const float DrawTime = 0.05f;                             // 디버그 선/도형이 남아있는 시간
		const FColor Col = bAnyHit ? FColor::Green : FColor::Red; // 디버그 색
		
		DrawDebugLine(GetWorld(), StartPos, EndPos, Col, false, DrawTime, 0, 2.f);                      // 라인	
		DrawDebugCapsule(GetWorld(), StartPos, HalfHeight, Radius, Rot, Col, false, DrawTime, 0, 1.5f); // 시작 캡슐
		DrawDebugCapsule(GetWorld(), EndPos, HalfHeight, Radius, Rot, Col, false, DrawTime, 0, 1.5f);   // 끝 캡슐

		// 
		if (bAnyHit)
		{
			// HitResults에 있는 충돌 결과를 하나씩 꺼내서 HR로 읽음
			for (const FHitResult& HR : HitResults)
			{
				// 맞은 액터를 뽑음
				AActor* HitActor = HR.GetActor();
				if (!IsValid(HitActor))
				{
					continue;
				}

				// 이번 스윙에서 이미 때렸으면 스킵
				if (HitActorsThisSwing.Contains(HitActor))
				{
					continue;
				}

				// 이번 스윙 첫 타격 등록
				HitActorsThisSwing.Add(HitActor);

				// 여기서 진짜 타격 처리(데미지/경직/넉백 등)
				UE_LOG(LogTemp, Log, TEXT("Hit ONCE ThisSwing: %s"), *HitActor->GetName());
			}
		}
	}
}
#pragma endregion

// 입력 바인딩
#pragma region Input binding
// 처음 한번만 실행 / 해당 키값에 따라 함수로 가게 세팅 / 축 ↔ 함수를 연결
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (!MoveAction || !LookAction || !JumpAction || !LookOnAction || !RunAction || !AttackAction)
		{
			UE_LOG(LogTemp, Error, TEXT("InputAction is null (Move/Look/Jump). Check IMC & Actions!"));
			return; // 널이면 바인딩 시도하지 말고 바로 반환
		}

		// 시야
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::MyCharacter_Look);

		// 점프
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::MyCharacter_Jump);

		// 락온
		EnhancedInput->BindAction(LookOnAction, ETriggerEvent::Started, this, &AMyCharacter::MyCharacter_LookOn);

		// 달리기
		EnhancedInput->BindAction(RunAction, ETriggerEvent::Started, this, &AMyCharacter::StartRun);
		EnhancedInput->BindAction(RunAction, ETriggerEvent::Completed, this, &AMyCharacter::StopRun);

		// 이동 & 제자리 회전
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Started, this, &AMyCharacter::MyCharacter_OnMoveStarted);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::MyCharacter_OnMoveTriggered);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyCharacter::MyCharacter_OnMoveCompleted);
		
		// 공격
		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &AMyCharacter::MyCharacter_Attack);
	}
}
#pragma endregion

// 달리기
#pragma region running
// 달리기 판정(Shift 누름)
void AMyCharacter::StartRun(const FInputActionValue& Value)
{
	// 점프 착지후 이동 잠금
	if (bMovementLocked || bAttacking)
	{
		return;
	}
	
	bRunKeyHeld = true;  // shift를 누름
}

// 달리기 판정(Shift 뗌)
void AMyCharacter::StopRun(const FInputActionValue& Value)
{
	bRunKeyHeld = false; // shift를 땜
}
#pragma endregion

// 시점 처리
#pragma region Point processing
void AMyCharacter::MyCharacter_Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();   // X=Yaw, Y=Pitch
	if (!Controller)
	{
		return;
	}
	
	// 입력감도
	const float yaw = Axis.X * MouseSensitivityYaw;
	const float pitch = Axis.Y * MouseSensitivityPitch * (bInvertY ? -1.f : 1.f);
	
	// 실제 전달
	AddControllerYawInput(yaw);
	AddControllerPitchInput(pitch);
	
	//각도 제한(위 아래)
	FRotator R = Controller->GetControlRotation();                        // 컨트롤러의 최종 회전값
	R.Normalize();                                                        // 정규화
	R.Pitch = FMath::ClampAngle(R.Pitch, CameraPitchMin, CameraPitchMax); // 위/아래 각도 제한
	Controller->SetControlRotation(R);                                    // 최종 적용
}
#pragma endregion

// 락온(카메라 회전)
#pragma region Lock-on (camera rotation)
void AMyCharacter::MyCharacter_LookOn(const FInputActionValue& Value)
{
	// 게임 시작 몽타지 중에는 회전 입력 무시
	if (bIntroLocked)
	{
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		LookOnTargetYaw = GetActorRotation().Yaw; // 캐릭터가 보는 Yaw를 목표로
		bLookOnInProgress = true;                 // 틱에서 보간 시작
	}
}
#pragma endregion

// 제자리 회전 & 기본 이동
#pragma region Rotate in place & basic movement
// 제자리 회전 시작: 자동회전 끄고, 이동속도 0으로 잠깐 묶어 "발 고정"
void AMyCharacter::BeginPivotToYaw(float TargetYawDeg)
{
	// 게임 시작 몽타지 중에는 회전 입력 무시
	if (bIntroLocked)
	{
		return; 
	}


	bPivotRotating = true;
	PivotTargetYaw = FMath::UnwindDegrees(TargetYawDeg);

	if (auto* Move = GetCharacterMovement())
	{
		// 피벗 중 자동 회전 금지
		Move->bOrientRotationToMovement = false;

		// 피벗 시작 순간 확실히 멈추기
		Move->StopMovementImmediately();
		ConsumeMovementInputVector();

		// 공격/이동락 중 아니면 발 고정
		if (!bMovementLocked && !bAttacking)
		{
			Move->MaxWalkSpeed = 0.f;
			Move->MaxAcceleration = 0.f;
		}
	}
}


// 제자리 회전 종료: 이동/자동회전 복구, 키가 여전히 눌려 있으면 즉시 이동
void AMyCharacter::FinishPivot()
{
	bPivotRotating = false; // 회전 모드 off

	if (auto* Move = GetCharacterMovement())
	{
		// 공격 중 또는 이동잠금 중이면 복구하지 않음
		if (!bMovementLocked && !bAttacking)
		{
			Move->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeedDefault; // 속도 복원
			Move->bOrientRotationToMovement = true;						   // 자동 회전 복구(이동 중엔 몸이 돌아감)
			Move->MaxAcceleration = bIsRunning ? RunAcceleration : WalkAcceleration;//
		}
	}

	// 키가 아직 눌려있다면 그 방향으로 바로 이동 시작
	if (bMoveKeyHeld && !LastMoveAxis.IsNearlyZero() && !bAttacking)
	{
		const FRotator YawOnly(0.f, GetActorRotation().Yaw, 0.f); // 현재 바라보는 방향 기준
		const FVector Forward = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::X); // 전/후 벡터
		const FVector Right = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::Y);   // 좌/우 벡터
		AddMovementInput(Forward, LastMoveAxis.Y);                // 입력 Y로 전/후 이동
		AddMovementInput(Right, LastMoveAxis.X);                  // 입력 X로 좌/우 이동
	}
}


// Move 누르는 "순간" : 회전 목표 계산하고 제자리 회전 시작
void AMyCharacter::MyCharacter_OnMoveStarted(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>(); // 현재 프레임의 이동 축(−1..1)
	LastMoveAxis = Axis;						   // 나중에 이동 재개 시 사용
	bMoveKeyHeld = !Axis.IsNearlyZero();		   // 키 눌림 상태 저장
	if (Axis.IsNearlyZero())					   // 미세 입력은 무시
	{
		return;
	}

	// 입력을 4방향 인덱스로(0:F,1:R,2:B,3:L)
	const int want = DesiredFacingFromAxis(Axis);
	if (want < 0)
	{
		return;
	}

	// 카메라 Yaw를 기준으로 목표 각도를 만듬
	float camYaw = Controller ? Controller->GetControlRotation().Yaw : GetActorRotation().Yaw;
	camYaw = FMath::UnwindDegrees(camYaw);

	// 90도 격자에 스냅 – 카메라가 애매한 각도여도 4방향이 똑 떨어지게
	const float baseYaw90 = 90.f * FMath::RoundToInt(camYaw / 90.f);

	// 입력 방향을 0/±90/180 오프셋으로 변환
	float offset = 0.f;           // Fwd
	switch (want)
	{
	case 1:  offset = 90.f; // Right
		break;
	case 2:  offset = 180.f; // Back
		break;
	case 3:  offset = -90.f; // Left
		break;
	default:                 // Forward(0)
		break;
	}

	// 최종 목표 Yaw = (카메라 기준) + 오프셋
	const float targetYaw = FMath::UnwindDegrees(baseYaw90 + offset);


	// 이미 그 방향을 거의 보고 있으면 피벗 생략(그냥 이동)
	const float currYaw = GetActorRotation().Yaw;
	const float deltaAbs = FMath::Abs(FMath::FindDeltaAngleDegrees(currYaw, targetYaw));
	if (deltaAbs <= PivotYawToleranceDeg)
	{
		// 스냅만 살짝 맞춰주고 종료
		SetActorRotation(FRotator(0.f, targetYaw, 0.f));
		return;
	}


	// 정말 필요할 때만 제자리 회전 시작
	BeginPivotToYaw(targetYaw);
}


// Move를 "누르고 있는 동안": 회전 중이면 무시, 끝났으면 일반 이동
void AMyCharacter::MyCharacter_OnMoveTriggered(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>(); // 지속 입력(프레임마다 들어옴)
	LastMoveAxis = Axis;                           // 최신 입력 저장
	bMoveKeyHeld = !Axis.IsNearlyZero();           // 현재도 누르고 있는지
	
	if (bPivotRotating)                            // 회전 중엔 이동 입력 무시(발 고정 유지)
	{
		return;
	}
	if (!Controller)							   // 소유 컨트롤러 없으면 무시
	{
		return;
	}
	
	// 카메라 Yaw 기준 전/후/좌/우 벡터               // 카메라를 기준으로 캐릭터 방향 제어
	const FRotator CamYaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector  Fwd = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::X);
	const FVector  Rt = FRotationMatrix(CamYaw).GetUnitAxis(EAxis::Y);
	
	// 캐릭터 이동
	AddMovementInput(Fwd, Axis.Y);                 // 전/후 이동
	AddMovementInput(Rt, Axis.X);                  // 좌/우 이동
}


// Move를 "뗀 순간": 홀드 상태 해제(회전 중이면 회전은 끝까지 계속)
void AMyCharacter::MyCharacter_OnMoveCompleted(const FInputActionValue& Value)
{
	LastMoveAxis = FVector2D::ZeroVector;                    // 입력 초기화
	bMoveKeyHeld = false;                                    // 홀드 해제
	// 회전 중이면 Tick에서 자연 종료되도록 둔다
}


// 현재 캐릭터의 Yaw를 4방향 인덱스(0:F,1:R,2:B,3:L)로 변환
int AMyCharacter::FacingFromYaw(float YawDeg)
{
	const float Y = FMath::UnwindDegrees(YawDeg); // 360 누적 각도를 -180~+180로 정리
	return (FMath::RoundToInt(Y / 90.f)) & 3;     // 90° 단위로 반올림 후 0..3으로 마스킹
}


// 입력 축을 4방향 인덱스(0:F,1:R,2:B,3:L)로 분류
int AMyCharacter::DesiredFacingFromAxis(const FVector2D& Axis)
{
	// 아주 약한 입력(미세 떨림) 무시
	constexpr float DeadZone = 0.2f;
	if (Axis.SizeSquared() < DeadZone * DeadZone) // 입력 없음
	{
		return -1;
	}
	// 좌우 절대값이 더 크면 수평 우선(D/A)
	if (FMath::Abs(Axis.X) > FMath::Abs(Axis.Y))
	{
		return (Axis.X > 0.f) ? 1 : 3;            // X>0이면 오른쪽(1), X<0이면 왼쪽(3)
	}

	// 그 외엔 전후(W/S)
	else if (!FMath::IsNearlyZero(Axis.Y))
	{
		return (Axis.Y > 0.f) ? 0 : 2;            // Y>0이면 정면(0), Y<0이면 뒤(2)
	}

	return -1;									  // 폴백(이론상 도달 안 하지만 경고 방지)
}
#pragma endregion

// 점프
#pragma region Jump
// 점프 실행 & 상태 바꿈
void AMyCharacter::MyCharacter_Jump()
{
	auto* AI = Cast<UMyCharacterAnimInstance>(AnimInstance);

	const bool bOnGround = GetCharacterMovement()->IsMovingOnGround();// 지상여부

	// 달리기 속도가 최대치에 근접할때만 점프 가능(속도를 최대치로 설정해두면 너무 빡빡함)
	if (bIsRunning == true && bLockJump == true && bOnGround && GetCharacterMovement()->Velocity.Size2D() >= 520.f)
	{
		Jump();
	}
}

// 점프 상태 On
void AMyCharacter::LockJump_On()
{
	bLockJump = true;
}

// 점프 상태 Off
void AMyCharacter::LockJump_Off()
{
	bLockJump = false;
}

void AMyCharacter::MoveLock_Off()
{
	// 점프 착지후 이동 잠금
	if (!bMovementLocked)
	{
		return;
	}

	bMovementLocked = false;// 락 끄기

	if (auto* Move = GetCharacterMovement())
	{
		// 달리는 중이면 달리기 값, 아니면 걷기 값으로 복원
		Move->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeedDefault;
		Move->MaxAcceleration = bIsRunning ? RunAcceleration : WalkAcceleration;

	}
}
void AMyCharacter::MoveLock_On()
{
	// 중복 호출 방지
	// 점프 착지후 이동 잠금
	if (bMovementLocked)
	{
		return;
	}

	bMovementLocked = true; // 락 키기

	if (auto* Move = GetCharacterMovement())
	{
		// 복구를 위해 현재값 백업
		SavedMaxWalkSpeed = Move->MaxWalkSpeed;
		SavedMaxAcceleration = Move->MaxAcceleration;

		// 즉시 정지 + 이후 이동 불가
		Move->StopMovementImmediately(); // 현재 속도 0으로
		Move->MaxWalkSpeed = 0.f;
		Move->MaxAcceleration = 0.f;

		// 남아있을 수 있는 대기 입력 비우기
		ConsumeMovementInputVector();
	}
}
#pragma endregion

// 공격 몽타지 실행
#pragma region Play attack montage
// 공격몽타지 실행
void AMyCharacter::MyCharacter_Attack(const FInputActionValue& Value)
{
	if (auto* AI = Cast<UMyCharacterAnimInstance>(AnimInstance))
	{
		// 떨어지는중이 아니고 && 점프 Recovery가 끝나고 && 게임시작 몽타지가 실행중이 아닐때 실행
		if (!GetCharacterMovement()->IsFalling() && bMovementLocked == false && !bIntroLocked)
		{
			// 공격 몽타지 실행 
			AI->PlayAttackMontage();
		}
	}
}
#pragma endregion

// 공격판정 On / Off 함수
#pragma region Attack judgment On / Off
void AMyCharacter::StartHit()
{
	bHitting = true;
	HitActorsThisSwing.Empty(); // 새 공격이 시작될 때, 이전 공격 기록 초기화
}

void AMyCharacter::EndHit()
{
	bHitting = false;
}
#pragma endregion