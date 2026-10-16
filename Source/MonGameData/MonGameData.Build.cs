// Copyright (c) mon-game. All rights reserved.

using UnrealBuildTool;

public class MonGameData : ModuleRules
{
	public MonGameData(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});
	}
}
