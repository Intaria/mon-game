// Copyright (c) mon-game. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MonGameEditorTarget : TargetRules
{
	public MonGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("MonGame");
	}
}
