using UnrealBuildTool;

public class RPGEquipmentUI : ModuleRules
{
    public RPGEquipmentUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UMG",
                "GameplayTags",
                "RPGCore",
                "RPGEquipment"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
               
                "Slate",
                "SlateCore",
                "InputCore"
            }
        );
    }
}