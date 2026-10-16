// Copyright (c) mon-game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataEnums.h"
#include "EntityId.h"
#include "NatureRow.generated.h"

/**
 * Row contract for Data/Battle/nature.csv (schema.md, table `nature`).
 * Natures deliberately carry no behavior modifiers yet: influence on behavior
 * is postponed (Docs/data/99-open-questions.md #9), and herd/sleep are species
 * and zone properties, not nature properties.
 */
USTRUCT()
struct MONGEDATA_API FNatureRow
{
	GENERATED_BODY()

	/** Stable id of the nature, e.g. "nature.hardy". */
	UPROPERTY(EditAnywhere, Category = "Nature")
	FEntityId NatureId;

	/** Localization key of the display name -> Data/Meta/locale.csv. */
	UPROPERTY(EditAnywhere, Category = "Nature")
	FString NameKey;

	/** Animation-set flavour; Default = the base animation set (no extra animations needed). */
	UPROPERTY(EditAnywhere, Category = "Nature")
	EAnimProfile AnimProfile = EAnimProfile::Default;

	/** Free-form flags for future rules without schema migration. */
	UPROPERTY(EditAnywhere, Category = "Nature")
	TArray<FString> Tags;
};
