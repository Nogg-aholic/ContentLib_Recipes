

#pragma once

#include "CoreMinimal.h"

#include "FGRecipe.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ContentLib_RecipesSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CONTENTLIB_RECIPES_API UContentLib_RecipesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadOnly)
		TMap<TSubclassOf<UFGRecipe>, FString> JsonRecipes;

	UPROPERTY(BlueprintReadOnly)
		TMap<TSubclassOf<UFGRecipe>, FString> JsonRecipePatches;
};
