// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyCharacter.h"

AMyGameModeBase::AMyGameModeBase()
{
    // DefaultPawnClass를 내 캐릭터 클래스로 지정
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Script/Engine.Blueprint'/Game/BluePrints/BP_MyCharacter.BP_MyCharacter_C'"));

	if (PlayerPawnClassFinder.Succeeded())
	{
		DefaultPawnClass = PlayerPawnClassFinder.Class;
	}
}