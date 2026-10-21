using UnrealBuildTool;

public class RPGDialogue : ModuleRules
{
	public RPGDialogue(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"RPGCore"
		});
	}
}
