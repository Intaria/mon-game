// Copyright (c) mon-game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EntityId.h"
#include "StatusRow.generated.h"

/**
 * Row contract for Data/Battle/status.csv (schema.md, table `status`).
 * Ways to cure a status are not stored here: they are universal `Rules/effect` rows
 * (cure_status) owned by items, abilities and NPC actions (decision №2).
 */
USTRUCT()
struct MONGEDATA_API FStatusRow
{
	GENERATED_BODY()

	/** Stable id of the status, e.g. "status.burn". */
	UPROPERTY(EditAnywhere, Category = "Status")
	FEntityId StatusId;

	/** Localization key of the display name -> Data/Meta/locale.csv. */
	UPROPERTY(EditAnywhere, Category = "Status")
	FString NameKey;

	/** How many battle turns the status lasts; 0 = does not expire on its own (schema.md). */
	UPROPERTY(EditAnywhere, Category = "Status")
	int32 DurationTurns = 0;

	/** GDD: the status survives the end of battle until cured. */
	UPROPERTY(EditAnywhere, Category = "Status")
	bool bPersistsAfterBattle = false;

	/** Chance to wear off at the end of each turn, in percent; 0 = never wears off by chance. */
	UPROPERTY(EditAnywhere, Category = "Status")
	int32 CureChancePercent = 0;

	/** Free-form flags for future rules without schema migration. */
	UPROPERTY(EditAnywhere, Category = "Status")
	TArray<FString> Tags;
};
