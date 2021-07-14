

#pragma once

#include "CoreMinimal.h"

#include "CLRecipe.h"
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

	UFUNCTION(BlueprintCallable)
	void FillLoadedClasses();
	UFUNCTION(BlueprintCallable)
	FContentLib_Recipe GenerateFromString(FString String);
	UFUNCTION(BlueprintCallable)
	FString SerializeRecipe(TSubclassOf<UFGRecipe> Recipe);
	UFUNCTION(BlueprintCallable)
	FString SerializeCLRecipe(FContentLib_Recipe Recipe);
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UCLRecipe> CreateContentLibRecipe(FString Name);

	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Items;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Builders;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> CraftingComps;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Schematics;
	
	UPROPERTY(BlueprintReadOnly)
		TMap<TSubclassOf<UCLRecipe>, FString> DynRecipes;

	UPROPERTY(BlueprintReadOnly)
		TMap<TSubclassOf<UFGRecipe>, FString> RecipePatches;
};
