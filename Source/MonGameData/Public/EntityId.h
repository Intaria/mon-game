// Copyright (c) mon-game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EntityId.generated.h"

/**
 * Stable data identifier: a namespaced slug such as "species.bulbasaur" or "type.fire"
 * (schema.md principle 2; decision №1).
 *
 * The syntax intentionally matches GameplayTags so a later adoption has no data migration,
 * but the data layer does not depend on the GameplayTags module — that decision belongs to
 * a gameplay layer. Validation is pure C++ because the loader and the data validator must
 * share exactly one implementation (AGENTS.md §4: pure logic stays engine-independent).
 */
USTRUCT()
struct MONGEDATA_API FEntityId
{
	GENERATED_BODY()

	FEntityId() = default;
	explicit FEntityId(FString InValue)
		: Value(MoveTemp(InValue))
	{
	}

	/** Raw slug text exactly as written in CSV. */
	UPROPERTY(EditAnywhere, Category = "Data|Id")
	FString Value;

	/** True when Value matches the namespaced-slug syntax. */
	bool IsValid() const
	{
		return IsValidSyntax(Value);
	}

	/** Parses and validates in one step; returns an empty optional for invalid input. */
	static TOptional<FEntityId> Parse(const FString& InValue);

	/** Syntax-only check: lowercase segments [a-z0-9_]+ joined by '.', at least two segments. */
	static bool IsValidSyntax(const FString& InValue);

	FString ToString() const
	{
		return Value;
	}

	bool operator==(const FEntityId& Other) const
	{
		return Value == Other.Value;
	}

	bool operator!=(const FEntityId& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FEntityId& Id)
	{
		return GetTypeHash(Id.Value);
	}
};
