// Copyright (c) mon-game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EntityId.h"
#include "TypeChartRow.generated.h"

/**
 * Row contract for Data/Types/type_chart.csv (schema.md, table `type_chart`).
 * The grid is stored in full (12x12 = 144 rows, decision №3); there is deliberately no
 * "missing row = 1.0" default so the validator fails on forgotten pairs instead of
 * silently balancing the game.
 */
USTRUCT()
struct MONGEDATA_API FTypeChartRow
{
	GENERATED_BODY()

	/** Attacker type (part of the composite primary key, FK -> `type`). */
	UPROPERTY(EditAnywhere, Category = "TypeChart")
	FEntityId AttackerTypeId;

	/** Defender type (part of the composite primary key, FK -> `type`). */
	UPROPERTY(EditAnywhere, Category = "TypeChart")
	FEntityId DefenderTypeId;

	/** Damage multiplier; the validator enforces the value set 0 / 0.25 / 0.5 / 1 / 2 / 4. */
	UPROPERTY(EditAnywhere, Category = "TypeChart")
	float Multiplier = 1.0f;
};
