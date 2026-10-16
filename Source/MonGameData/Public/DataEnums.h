// Copyright (c) mon-game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataEnums.generated.h"

/** ability.kind — damage channel of an ability (schema.md, table `ability`). */
UENUM()
enum class EAbilityKind : uint8
{
	Physical,
	Energy
};

/**
 * ability.target_scope — who the ability can be applied to.
 * Battle-side resolution (ally->self in 1v1, target choice in 2v2) lives in code,
 * not in data (schema.md, decision №17 round 15.10).
 */
UENUM()
enum class ETargetScope : uint8
{
	Self,
	Enemy,
	Ally,
	Any,
	All,
	None,
	Random
};

/** nature.anim_profile — animation-set flavour; Default = the base animation set. */
UENUM()
enum class EAnimProfile : uint8
{
	Default,
	Angry,
	Happy,
	Lazy
};
