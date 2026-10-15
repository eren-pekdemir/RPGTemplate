using UnrealBuildTool;

public class RPGCraftingUI : ModuleRules
{
    public RPGCraftingUI(ReadOnlyTargetRules Target) : base(Target)
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
            "RPGCrafting"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
            "InputCore"
        });
    }
}