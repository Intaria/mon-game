// Copyright (c) mon-game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataEnums.h"
#include "EntityId.h"
#include "AbilityRow.generated.h"

/**
 * Row contract for Data/Abilities/ability.csv (schema.md, table `ability`).
 * Side effects, miss chances and out-of-battle applications are universal `Rules/effect`
 * rows owned by the ability (decision №2); learn requirements are `Rules/condition` rows.
 * Field defaults mirror the schema "empty CSV cell = default" rule (decision №15).
 */
USTRUCT()
struct MONGEDATA_API FAbilityRow
{
	GENERATED_BODY()

	/** Stable id of the ability, e.g. "ability.flamethrower". */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FEntityId AbilityId;

	/** Localization key of the display name -> Data/Meta/locale.csv. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FString NameKey;

	/** Localization key of the description -> Data/Meta/locale.csv. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FString DescKey;

	/** Exactly one type per ability (GDD); FK -> `type`. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FEntityId TypeId;

	/** Physical or energy damage channel. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	EAbilityKind Kind = EAbilityKind::Physical;

	/** Base damage before multipliers; the formula itself lives in C++ (principle 6). */
	UPROPERTY(EditAnywhere, Category = "Ability")
	int32 BasePower = 0;

	/** Hit chance in percent; 100 = never misses by accuracy alone. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	int32 Accuracy = 100;

	/** Recharge in battle turns; 0 = no cooldown. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	int32 CooldownTurns = 0;

	/** Wind-up in battle turns; 0 = resolves immediately. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	int32 CastTurns = 0;

	/** Recovery after use, in battle turns; 0 = none. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	int32 RecoveryTurns = 0;

	/**
	 * Who the ability can be applied to. Battle-side resolution
	 * (ally -> self in 1v1, target choice in 2v2) lives in the resolver code, not in data.
	 */
	UPROPERTY(EditAnywhere, Category = "Ability")
	ETargetScope TargetScope = ETargetScope::Enemy;

	/** May be applied in the world, outside battle (field skills such as teleport). */
	UPROPERTY(EditAnywhere, Category = "Ability")
	bool bUsableInField = false;

	/** Name of the C++ formula hook; parameters live in Balance/gameplay_config.csv (principle 6). */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FString FormulaRef;

	/** Free-form flags for future rules without schema migration. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	TArray<FString> Tags;
};
