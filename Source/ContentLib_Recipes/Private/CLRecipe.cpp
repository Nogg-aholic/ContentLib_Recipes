


#include "CLRecipe.h"
#include "FGSchematic.h"
#include "FGSchematicManager.h"
#include "FGUnlockSubsystem.h"
#include "FGWorkBench.h"
#include "ContentLib_RecipesSubsystem.h"
#include "Buildables/FGBuildableFactory.h"
#include "Registry/ModContentRegistry.h"
#include "Unlocks/FGUnlockRecipe.h"

void UCLRecipe::InitFromStruct(UContentLib_RecipesSubsystem* Subsystem ,FContentLib_Recipe RecipeStruct, TSubclassOf<class UFGRecipe> Recipe,bool ClearIngredients,bool ClearProducts,bool ClearBuilders)
{
	if (!Recipe)
		return;
	UFGRecipe *  CDO = Recipe.GetDefaultObject();

	if (RecipeStruct.Name != "")
	{
		CDO->mDisplayName = FText::FromString(RecipeStruct.Name);
		CDO->mDisplayNameOverride = true;
	}
	AddProductOrIngredient(Recipe,RecipeStruct,Subsystem->Items,true, ClearIngredients);
	AddProductOrIngredient(Recipe,RecipeStruct,Subsystem->Items,false, ClearProducts);
	AddBuilders(Recipe,RecipeStruct,Subsystem->Builders,Subsystem->CraftingComps, ClearBuilders);
	AddToSchematicUnlock(Recipe,RecipeStruct,Subsystem);
	if(RecipeStruct.ManufacturingDuration != 1)
		CDO->mManufactoringDuration = RecipeStruct.ManufacturingDuration;
	if (RecipeStruct.ManualManufacturingMultiplier != 1)
		CDO->mManualManufacturingMultiplier = RecipeStruct.ManualManufacturingMultiplier;
	if (RecipeStruct.VariablePowerConsumptionConstant != 0)
		CDO->mVariablePowerConsumptionConstant = RecipeStruct.VariablePowerConsumptionConstant;
	if (RecipeStruct.VariablePowerConsumptionFactor != 1)
		CDO->mVariablePowerConsumptionFactor = RecipeStruct.VariablePowerConsumptionFactor;
	if (RecipeStruct.ManufacturingMenuPriority != 0)
		CDO->mManufacturingMenuPriority = RecipeStruct.ManufacturingMenuPriority;

}

void UCLRecipe::AddProductOrIngredient(const TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Items, bool Ingredient, bool ClearFirst)
{
	if (!Recipe)
		return;
	if (ClearFirst)
	{
		if (Ingredient)
			Recipe.GetDefaultObject()->mIngredients.Empty();
		else
			Recipe.GetDefaultObject()->mProduct.Empty();
	}
	
	for(auto i : Ingredient ? RecipeStruct.Ingredients : RecipeStruct.Products)
	{
		for(auto e : Items)
		{	
			TSubclassOf<class UFGItemDescriptor> Desc = e;
			FString S = Desc->GetName();
			FString Tp = i.Key;
			FString DescPre = FString("Desc_").Append(i.Key);
			if (!i.Key.EndsWith("_C"))
			{
				DescPre.Append("_C");
			}
			if(S.Equals(i.Key,ESearchCase::IgnoreCase) || S.Equals(Tp.Append("_C"), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
			{
				FItemAmount Amount;
				Amount.Amount = i.Value;
				Amount.ItemClass = Desc;
				auto & Target = Ingredient ? Recipe.GetDefaultObject()->mIngredients : Recipe.GetDefaultObject()->mProduct;
				bool Exists = false;
				for(auto & l : Target)
				{
					if(l.ItemClass == Desc)
					{
						Exists = true;
						if(l.Amount != Amount.Amount)
						{
							l.Amount = Amount.Amount;
						}
						break;
					}
				}
				if(!Exists)
					Target.Add(Amount);
				break;
			}
		}
	}
}

void UCLRecipe::AddBuilders(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Builders,TArray<UClass*> CraftingComps, const bool ClearFirst)
{
	if (!Recipe)
		return;
	if (ClearFirst)
		Recipe.GetDefaultObject()->mProducedIn.Empty();

	for(auto i : RecipeStruct.BuildIn)
	{
		for(auto e : Builders)
		{
			FString S = e->GetName();
			FString A = i;
			FString DescPre = FString("Build_").Append(i);
			if (!DescPre.EndsWith("_C"))
			{
				DescPre.Append("_C");
			}
			if (S.Equals(i, ESearchCase::IgnoreCase) || S.Equals(A.Append("_C"), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
			{
				TSoftClassPtr< UObject > Insert = TSoftClassPtr< UObject > (e);
				if(!Recipe.GetDefaultObject()->mProducedIn.Contains((Insert)))
					Recipe.GetDefaultObject()->mProducedIn.Add(Insert);
				break;
			}
		}
		for(auto e : CraftingComps)
		{
			TSubclassOf<class UFGWorkBench> Desc = e;
			if(Desc.GetDefaultObject()->GetName() == i || i == "manual")
			{
				TSoftClassPtr< UObject > Insert = TSoftClassPtr< UObject > (e);
				if(!Recipe.GetDefaultObject()->mProducedIn.Contains((Insert)))
					Recipe.GetDefaultObject()->mProducedIn.Add(Insert);
				break;
			}
		}
		if (i.Contains("BP_BuildGun") || i.Contains("BuildGun") || i.Contains("FGBuildGun"))
		{
			UClass* Class = LoadClass<UClass>(nullptr, TEXT("/Game/FactoryGame/Equipment/BuildGun/BP_BuildGun.BP_BuildGun_C"));
			if (!Recipe.GetDefaultObject()->mProducedIn.Contains((Class)))
				Recipe.GetDefaultObject()->mProducedIn.Add(Class);
		}
	}
}

void UCLRecipe::AddToSchematicUnlock(const TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct, UContentLib_RecipesSubsystem* Subsystem)
{
	if (!Recipe)
		return;
	for (auto i : RecipeStruct.UnlockedBy)
	{
		for(auto e : Subsystem->Schematics)
		{
			FString S = e->GetName();
			FString A = i;
			if (S.Equals(i, ESearchCase::IgnoreCase) || S.Equals(A.Append("_C"), ESearchCase::IgnoreCase))
			{
				TSubclassOf<UFGSchematic> Schematic = e;
				AFGSchematicManager * Manager = AFGSchematicManager::Get(Subsystem->GetWorld());
				AFGUnlockSubsystem * Unlock = AFGUnlockSubsystem::Get(Subsystem->GetWorld());
				bool Added = false;
				for(auto f : Schematic.GetDefaultObject()->mUnlocks)
				{
					if(Cast<UFGUnlockRecipe>(f))
					{
						if (!Cast<UFGUnlockRecipe>(f)->mRecipes.Contains(Recipe))
						{
							Cast<UFGUnlockRecipe>(f)->mRecipes.Add(Recipe);
							Added = true;
						}
					}
				}
				if (!Added)
				{
					UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockRecipe_C"), false);
					if (!Class)
					{
						Class = LoadClass<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockRecipe.BP_UnlockRecipe_C"));
						if (!Class)
						{
							UE_LOG(LogTemp,Fatal,TEXT("CONTENTLIBRECIPES: Couldnt find BP_UnlockRecipe_C wanting to Add to %s"), *Schematic->GetName())
						}
					}
					UFGUnlockRecipe* Object = NewObject<UFGUnlockRecipe>(Schematic.GetDefaultObject(),Class);
					Object->mRecipes.Add(Recipe);
					Schematic.GetDefaultObject()->mUnlocks.Add(Object);
				}
				if (Manager && Unlock && Unlock->HasAuthority())
				{
					if(Manager->IsSchematicPurchased(e))
					{
						Unlock->UnlockRecipe(Recipe);
					}
				}
			}
		}
	}
};
