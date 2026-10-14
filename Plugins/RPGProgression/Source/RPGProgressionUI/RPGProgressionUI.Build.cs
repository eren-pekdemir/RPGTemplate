using UnrealBuildTool;

public class RPGProgressionUI : ModuleRules
{
    public RPGProgressionUI(ReadOnlyTargetRules Target) : base(Target)
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
                "RPGProgression"
                
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "InputCore",
                "Slate",
                "SlateCore"
            }
        );
    }
}