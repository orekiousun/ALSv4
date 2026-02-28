// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ALSv4 : ModuleRules
{
	public ALSv4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
