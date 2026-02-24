// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class A_Project : ModuleRules
{
	public A_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",                    // 기본 타입(FString, TArray 등)       
			"CoreUObject",			   // UObject, UCLASS, 리플렉션
			"Engine",				   // Actor, World, Component
			"InputCore",			   // 키보드/마우스 입력
			"EnhancedInput",           // 네가 쓰는 IMC / InputAction
			"AIModule",                // AIController, BehaviorTree
			"StateTreeModule",         // UE5 StateTree
			"GameplayStateTreeModule", // 게임플레이용 StateTree
			"UMG",					   // UI (위젯)
			"Slate",				   // 저수준 UI 시스템
			"GameplayTasks",           // AI/게임플레이 태스크 시스템
			"NavigationSystem"         // 네비메시 기반 경로 탐색 및 AI 이동
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

        // 헤더 검색 경로
        PublicIncludePaths.AddRange(new string[] {
			"A_Project",
			"A_Project/Variant_Platforming",
			"A_Project/Variant_Platforming/Animation",
			"A_Project/Variant_Combat",
			"A_Project/Variant_Combat/AI",
			"A_Project/Variant_Combat/Animation",
			"A_Project/Variant_Combat/Gameplay",
			"A_Project/Variant_Combat/Interfaces",
			"A_Project/Variant_Combat/UI",
			"A_Project/Variant_SideScrolling",
			"A_Project/Variant_SideScrolling/AI",
			"A_Project/Variant_SideScrolling/Gameplay",
			"A_Project/Variant_SideScrolling/Interfaces",
			"A_Project/Variant_SideScrolling/UI"
		});

        // Slate UI를 C++로 직접 쓸 때만 필요
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // 멀티플레이/온라인 기능 쓸 때만
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}