// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterAnimInstance.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// 생성자
#pragma region Constructor
UMyCharacterAnimInstance::UMyCharacterAnimInstance()
{
	// 공격 몽타지 세팅
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_A1(TEXT("/Script/Engine.AnimMontage'/Game/Montage/Attack_PrimaryA_Montage.Attack_PrimaryA_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_A2(TEXT("/Script/Engine.AnimMontage'/Game/Montage/Attack_PrimaryB_Montage.Attack_PrimaryB_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_A3(TEXT("/Script/Engine.AnimMontage'/Game/Montage/Attack_PrimaryC_Montage.Attack_PrimaryC_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> GSM(TEXT("/Script/Engine.AnimMontage'/Game/Montage/LevelStart_Montage.LevelStart_Montage'"));

	if (AM_A1.Succeeded() && AM_A2.Succeeded() && AM_A3.Succeeded() && GSM.Succeeded())
	{
		AttackMontage_1 = AM_A1.Object;
		AttackMontage_2 = AM_A2.Object;
		AttackMontage_3 = AM_A3.Object;
		GameStartMontage = GSM.Object;
	}
}
#pragma endregion

#pragma region NativeInitializeAnimation()
// 애님 인스턴스 생성 시점에 1회
void UMyCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}
#pragma endregion
#pragma region NativeBeginPlay()
// 플레이 시작 시점 1회
void UMyCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// 이동 컴포넌트 캐싱 / 속도, 가속, 지면여부 읽기가능
	auto Pawn = TryGetPawnOwner();                                 // 애님 인스턴스가 붙은 Pawn 얻기
	if (IsValid(Pawn))
	{
		MyCharacter = Cast<AMyCharacter>(Pawn);
		if (IsValid(MyCharacter))
		{
			CharacterMovement = MyCharacter->GetCharacterMovement();// 이동 컴포넌트 캐싱
		}
	}
}
#pragma endregion
#pragma region NativeUpdateAnimation()
// 매 프레임 호출
void UMyCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(CharacterMovement))
	{
		Velocity = CharacterMovement->Velocity;							  // 현재 속도(월드 기준)
		GroundSpeed = Velocity.Size2D();							      // 수평 속도 크기(단위: cm/s)
		auto Acceleration = CharacterMovement->GetCurrentAcceleration();  // 가속도 벡터

		ShouldMove = (GroundSpeed > 3.0f) && !Acceleration.IsNearlyZero(); // 이동 싱태 판정

		Running = MyCharacter->bIsRunning;                                // 달리기 상태 

		IsFalling = CharacterMovement->IsFalling();                       // 떨어지는 상태 판정

		bLockJumpAnim = MyCharacter->bLockJump;                           // 점프 가능한 상태

		// Vertical & Horizontal
		FRotator Rotation = MyCharacter->GetActorRotation();
		FVector UnrotateVector = Rotation.UnrotateVector(Velocity);

		// 벡터 길이를 1로 정규화해 방향만 남김
		UnrotateVector.Normalize();    // 정규화

		// Blend Space 2D에 들어갈 최종 축 값
		Vertical = UnrotateVector.X;   // 전/후(-1..1)
		Horizontal = UnrotateVector.Y; // 좌/우(-1..1)
	}
}
#pragma endregion

// 게임시작 몽타지 실행
#pragma region GameStartMontage_Play
void UMyCharacterAnimInstance::PlayGameStartMontage()
{
	Montage_Play(GameStartMontage, 1.f);
}
#pragma endregion

// 공격 몽타지 & 콤보
#pragma region Attack Montage & Combo
// 공격 입력 처리(캐릭터가 호출)
void UMyCharacterAnimInstance::PlayAttackMontage()
{
	if (!IsAttackMontagePlaying())
	{
		ComboStep = 1;
		bQueuedNext = false;
		bBufferedAttack = false;
		bComboWindowOpen = false;
		PlayStep(ComboStep);
		return;
	}

	// 노피타이가 열리고 나서 공격할 경우
	if (bComboWindowOpen)
	{
		bQueuedNext = true;
	}
	//노티파이가 열리기전 공격할 경우
	else
	{
		bBufferedAttack = true;
		BufferedAttackExpireTime = GetWorld()->GetTimeSeconds()// 현재 시간(게임 타임)
			+ BufferDuration;
	}


	UE_LOG(LogTemp, Warning, TEXT("[Input] Step=%d Window=%d Queued=%d"),
		ComboStep, bComboWindowOpen, bQueuedNext);
}

// SaveAttack 노티파이에서 호출 & 콤보창 열기
void UMyCharacterAnimInstance::OpenComboWindow()
{
	bComboWindowOpen = true;

	// 창이 열리기전 공격 && 콤보 창이 열리기 0.15초 안에 눌렀을 경우
	if (bBufferedAttack && GetWorld()->GetTimeSeconds() <= BufferedAttackExpireTime)
	{
		bQueuedNext = true;
	}

	bBufferedAttack = false;


	UE_LOG(LogTemp, Warning, TEXT("[Notify] OpenWindow Step=%d"), ComboStep);
}

// ResetCombo 노티파이에서 호출 & 콤보창 닫기
void UMyCharacterAnimInstance::CloseComboWindowAndBranch()
{
	bComboWindowOpen = false;

	// 3타(마지막 타)면 여기서 콤보 종료
	if (ComboStep >= 3)
	{
		ClearCombo();
		return;
	}

	const bool bShouldChain = bQueuedNext || bBufferedAttack;

	if (bShouldChain)
	{
		bQueuedNext = false;
		bBufferedAttack = false;

		ComboStep = FMath::Clamp(ComboStep + 1, 1, 3);
		PlayStep(ComboStep);
	}
	else
	{
		ClearCombo();
	}


	UE_LOG(LogTemp, Warning, TEXT("[Notify] CloseWindow Step=%d Queued=%d"), ComboStep, bQueuedNext);
}

// 공격 콤보에 따른 몽타지 실행
void UMyCharacterAnimInstance::PlayStep(int32 Step)
{
	switch (Step)
	{
	case 1:
		if (AttackMontage_1)
		{
			Montage_Play(AttackMontage_1, 1.f);
			UE_LOG(LogTemp, Log, TEXT("attack_1"));
		}
		break;
	case 2:
		if (AttackMontage_2)
		{
			Montage_Play(AttackMontage_2, 1.f);
			UE_LOG(LogTemp, Log, TEXT("attack_2"));
		}
		break;
	case 3:
		if (AttackMontage_3)
		{
			Montage_Play(AttackMontage_3, 1.f);
			UE_LOG(LogTemp, Log, TEXT("attack_3"));
		}
		break;
	default:
		break;
	}
}

// 콤보변수 초기화
void UMyCharacterAnimInstance::ClearCombo()
{
	ComboStep = 0;
	bQueuedNext = false;
	bComboWindowOpen = false;
	bBufferedAttack = false;
}
#pragma endregion

// 공격 몽타지 실행 중인지 판별
#pragma region Attack montage identification
bool UMyCharacterAnimInstance::IsAttackMontagePlaying() const
{
	// 공격 몽타지 실행 중이면 리턴 1
	if ((AttackMontage_1 && Montage_IsPlaying(AttackMontage_1))
		|| (AttackMontage_2 && Montage_IsPlaying(AttackMontage_2))
		|| (AttackMontage_3 && Montage_IsPlaying(AttackMontage_3)))
	{
		return true;
	}
	// 공격 몽타지 실행 중이 아니면 리턴 0
	else
	{
		return false;
	}
}
#pragma endregion

// 공격 판정 On/Off 함수
#pragma region Attack judgment On / Off
// 공격 판정 On 
void UMyCharacterAnimInstance::AnimNotify_StartHit()
{
	MyCharacter->StartHit();
}

// 공격 판정 Off
void UMyCharacterAnimInstance::AnimNotify_EndHit()
{
	MyCharacter->EndHit();
}
#pragma endregion