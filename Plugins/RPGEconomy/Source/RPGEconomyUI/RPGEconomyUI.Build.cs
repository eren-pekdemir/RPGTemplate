using UnrealBuildTool;

public class RPGEconomyUI : ModuleRules
{
	public RPGEconomyUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
			"GameplayTags",
			"RPGCore",
			"RPGEconomy"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"InputCore"
		});
	}
}
