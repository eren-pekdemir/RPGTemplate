// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPGInteraction : ModuleRules
{
	public RPGInteraction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RPGCore",
				
			}
			);
	}
}
