// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BigMoxiGames : ModuleRules
{
	public BigMoxiGames(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", 
			"CoreAS", 
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"CoreUObject", 
			"OnlineServicesNull",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"Engine", 
			"InputCore", 
			"EnhancedInput" });
	}
}
