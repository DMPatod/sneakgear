using UnrealBuildTool;

public class SneakGearTests : ModuleRules
{
	public SneakGearTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"AIModule",
			"UMG",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"SneakGear",
			"SneakGearUI",
			"SneakGearCore"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"UnrealEd"
		});
	}
}
