// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAnimInstance.h" // 커스텀 애님 인스턴스
#include "EnemyAIController.h" // AIController
#include "GameFramework/CharacterMovementComponent.h" // CharacterMovementComponent 쓰기

// 생성자
#pragma region Constructor
AEnemy::AEnemy()
{
	// TiCK함수 사용여부 / 필요하지 않으면 성능 향상을 위해 이 기능을 끌 수 있음
	PrimaryActorTick.bCanEverTick = true;

	// 에셋 경로로 메쉬 로드
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MS(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonSevarog/Characters/Heroes/Sevarog/Meshes/Sevarog.Sevarog'"));
	if (MS.Succeeded())
	{
		//매쉬 할당
		GetMesh()->SetSkeletalMesh(MS.Object);
	
		// 위치 및 회전 보정 (캡슐 콜리전과 맞추기 위함)
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	// 애님 블루프린트 클래스를 찾아 메쉬에 연결 / 블루프린트 경로를 가져올때는 마지막에 _C *필수*
	static ConstructorHelpers::FClassFinder<UAnimInstance> AI(TEXT("/Script/Engine.AnimBlueprint'/Game/Animation/ABP_Enemy.ABP_Enemy_C'"));
	if (AI.Succeeded())
	{
		// 할당
		GetMesh()->SetAnimInstanceClass(AI.Class);
	}

	AIControllerClass = AEnemyAIController::StaticClass(); // AIController 연결

	auto* Move = GetCharacterMovement();

	// 캐릭터 기본이동
	Move->MaxWalkSpeed = 300.f; // 기본 걷기 속도(기존 600)
}
#pragma endregion

// 처음 시작
#pragma region BeginPlay()
void AEnemy::BeginPlay()
{
	// 부모 클래스 초기화
	Super::BeginPlay();

	// 애님 인스턴스(실행객체)를 캐싱
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	// 몽타지와 OnAttackMontageEnded함수를 연결
	EnemyAnimInstance->OnMontageEnded.AddDynamic(this, &AEnemy::OnAttackMontageEnded);

	if (!EnemyAnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemy::BeginPlay: Failed to cast AnimInstance to UEnemyAnimInstance. Check ABP_Enemy parent class!"));
	}

	
}
#pragma endregion

#pragma region Tick()
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
#pragma endregion

// 입력 바인딩
#pragma region Input binding
// 처음 한번만 실행 / 해당 키값에 따라 함수로 가게 세팅 / 축 ↔ 함수를 연결
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

#pragma endregion

// 공격 몽타지 실행
#pragma region Play attack montage
void AEnemy::Enemy_Attack()
{
	if (!bIsAttacking)
	{
		bIsAttacking = true;

		if (IsValid(EnemyAnimInstance))
		{
			if (!bAttack_1)
			{
				// 1타
				EnemyAnimInstance->Enemy_PlayAttackMontage_1();
			}
			else
			{
				// 2타
				EnemyAnimInstance->Enemy_PlayAttackMontage_2();
			}

			// 공격 타 바꾸기
			bAttack_1 = !bAttack_1;
		}
	}
}
#pragma endregion

// 공격 몽타지 끝 실행 함수
#pragma region PlayAttackMontageEnd
void AEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;
}
#pragma endregion