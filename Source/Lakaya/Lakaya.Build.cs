// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lakaya : ModuleRules
{
	public Lakaya(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput","OnlineSubsystem","OnlineSubsystemEOS", "UMG", "AIModule", "NavigationSystem", "Niagara", "SlateCore" });
	}
}
