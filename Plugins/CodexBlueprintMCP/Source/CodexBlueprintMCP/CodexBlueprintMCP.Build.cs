using UnrealBuildTool;

public class CodexBlueprintMCP : ModuleRules
{
	public CodexBlueprintMCP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AssetRegistry",
			"BlueprintGraph",
			"EditorScriptingUtilities",
			"HTTPServer",
			"Json",
			"JsonUtilities",
			"KismetCompiler",
			"Projects",
			"UnrealEd"
		});
	}
}
