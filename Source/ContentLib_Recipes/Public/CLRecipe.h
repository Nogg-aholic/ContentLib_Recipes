

#pragma once

#include "CoreMinimal.h"
#include "FGRecipe.h"
#include "CLRecipe.generated.h"

class UContentLib_RecipesSubsystem;

class UFGSchematic;
USTRUCT(BlueprintType)
struct  CONTENTLIB_RECIPES_API  FContentLib_Recipe
{
	GENERATED_BODY()
	FContentLib_Recipe(): OverrideName(-1),
	                      ManufacturingDuration(-1),
	                      ManualManufacturingMultiplier(-1),
	                      VariablePowerConsumptionFactor(-1),
	                      VariablePowerConsumptionConstant(-1),
	                      ManufacturingMenuPriority(-1),
	                      ClearIngredients(true),
	                      ClearProducts(true),
	                      ClearBuilders(true)
	{
	} ;

	UPROPERTY(BlueprintReadWrite)
		int32 OverrideName;

	UPROPERTY(BlueprintReadWrite)
		FString Name;

	UPROPERTY(BlueprintReadWrite)
		FString Category;

	UPROPERTY(BlueprintReadWrite)
		TMap<FString, int32> Ingredients;
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, int32> Products;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> BuildIn;

	UPROPERTY(BlueprintReadWrite)
		TArray<FString> UnlockedBy;
	UPROPERTY(BlueprintReadWrite)
		float ManufacturingDuration;
	UPROPERTY(BlueprintReadWrite)
		float ManualManufacturingMultiplier;
	UPROPERTY(BlueprintReadWrite)
		float VariablePowerConsumptionFactor;
	UPROPERTY(BlueprintReadWrite)
		float VariablePowerConsumptionConstant;
	UPROPERTY(BlueprintReadWrite)
		float ManufacturingMenuPriority;
	UPROPERTY(BlueprintReadWrite)
		bool ClearIngredients;
	UPROPERTY(BlueprintReadWrite)
		bool ClearProducts;
	UPROPERTY(BlueprintReadWrite)
		bool ClearBuilders;
};

/**
 * 
 */
UCLASS()
class CONTENTLIB_RECIPES_API UCLRecipe : public UFGRecipe
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static void InitFromStruct(UContentLib_RecipesSubsystem* Subsystem ,FContentLib_Recipe RecipeStruct, TSubclassOf<class UFGRecipe> Recipe, bool ClearIngredients = true, bool ClearProducts = true, bool ClearBuilders = true);
	UFUNCTION(BlueprintCallable)
	static void AddToSchematicUnlock(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct, UContentLib_RecipesSubsystem* Subsystem);
	void AddToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLib_RecipesSubsystem* Subsystem);
	UFUNCTION(BlueprintCallable)
	static void AddBuilders(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Builders,TArray<UClass*> CraftingComps, bool ClearFirst = false);
	UFUNCTION(BlueprintCallable)
	static void AddProductOrIngredient(TSubclassOf<class UFGRecipe> Recipe, FContentLib_Recipe RecipeStruct, TArray<UClass*> Items, bool Ingredient, bool ClearFirst = false);
};
