// Copyright (c) mon-game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EntityId.h"
#include "TypeRow.generated.h"

/**
 * Row contract for Data/Types/type.csv (schema.md, table `type`).
 * Reflection is deliberate: the row mirrors the CSV contract (ADR-003) and stays importable
 * into a DataTable as a derived artifact; gameplay rules never live in this type
 * (AGENTS.md §4 — policy 1).
 */
USTRUCT()
struct MONGEDATA_API FTypeRow
{
	GENERATED_BODY()

	/** Stable id of the type, e.g. "type.fire". */
	UPROPERTY(EditAnywhere, Category = "Type")
	FEntityId TypeId;

	/** Localization key of the display name -> Data/Meta/locale.csv. */
	UPROPERTY(EditAnywhere, Category = "Type")
	FString NameKey;

	/** Palette key resolved by the UI; a single reference color per type. */
	UPROPERTY(EditAnywhere, Category = "Type")
	FString ColorKey;

	/** Free-form flags for future rules without schema migration. */
	UPROPERTY(EditAnywhere, Category = "Type")
	TArray<FString> Tags;
};
