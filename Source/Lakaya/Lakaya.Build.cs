// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lakaya : ModuleRules
{
	public Lakaya(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "OnlineSubsystem",
			"OnlineSubsystemNull","OnlineSubsystemUtils", "UMG", "AIModule", "NavigationSystem", "Niagara", "SlateCore", "Sockets", "AdvancedSessions",
			"Networking", "Json", "Agones", "GameplayAbilities", "GameplayTags", "GameplayTasks", "NetCore", "AdvancedWidgets", "CommonUI"
		});
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}