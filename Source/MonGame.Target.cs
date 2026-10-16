// Copyright (c) mon-game. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MonGameTarget : TargetRules
{
	public MonGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("MonGame");
		ExtraModuleNames.Add("MonGameData");
	}
}
