

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


	void FillLoadedClasses();
	
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Items;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Builders;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> CraftingComps;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Schematics;
	
	UPROPERTY(BlueprintReadOnly)
		TMap<TSubclassOf<UFGRecipe>, FString> JsonRecipes;

	UPROPERTY(BlueprintReadOnly)
		TMap<TSubclassOf<UFGRecipe>, FString> JsonRecipePatches;
};
