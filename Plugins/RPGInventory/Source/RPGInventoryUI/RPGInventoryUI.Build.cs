using UnrealBuildTool;

public class RPGInventoryUI : ModuleRules
{
    public RPGInventoryUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UMG",
                "RPGCore",
                "RPGInventory",
                "GameplayTags"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                
                "Slate",
                "SlateCore"
            }
        );
    }
}