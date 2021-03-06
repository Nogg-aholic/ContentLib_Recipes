


#include "ContentLib_RecipesSubsystem.h"
#include "FGSchematic.h"
#include "FGWorkBench.h"
#include "FGItemCategory.h"
#include "Buildables/FGBuildableFactory.h"

void UContentLib_RecipesSubsystem::FillLoadedClasses()
{
	GetDerivedClasses(UFGItemDescriptor::StaticClass(), Items, true);
	GetDerivedClasses(AFGBuildableFactory::StaticClass(), Builders, true);
	GetDerivedClasses(UFGWorkBench::StaticClass(), CraftingComps, true);
	GetDerivedClasses(UFGSchematic::StaticClass(), Schematics, true);
	GetDerivedClasses(UFGItemCategory::StaticClass(), ItemCategories, true);
}

FContentLib_Recipe UContentLib_RecipesSubsystem::GenerateFromString(FString String)
{
	if (String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
	{
		if (String == "")
			UE_LOG(LogTemp, Error, TEXT("Empty String  %s"), *String)
		else if (!String.StartsWith("{"))
			UE_LOG(LogTemp, Error, TEXT("String doesnt start with '{' %s"), *String)
		else if (!String.EndsWith("}"))
			UE_LOG(LogTemp, Error, TEXT("String doesnt end with '}'  %s"), *String);

		return FContentLib_Recipe();
	}
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if(!Result.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Json ! %s"), *String);
		return FContentLib_Recipe();
	}

	FContentLib_Recipe Recipe;
	if(Result->HasField("Name") && Result->TryGetField("Name")->Type == EJson::String)
	{
		Recipe.Name = Result->TryGetField("Name")->AsString();
	}
	if (Result->HasField("OverrideName") && Result->TryGetField("OverrideName")->Type == EJson::Boolean)
	{
		Recipe.OverrideName = static_cast<int32>(Result->TryGetField("OverrideName")->AsBool());
	}
	if (Result->HasField("Category") && Result->TryGetField("Category")->Type == EJson::String)
	{
		Recipe.Category = Result->TryGetField("Category")->AsString();
	}
	if (Result->HasField("ManufacturingDuration") && Result->TryGetField("ManufacturingDuration")->Type == EJson::Number)
	{
		Recipe.ManufacturingDuration = Result->TryGetField("ManufacturingDuration")->AsNumber();
	}
	if (Result->HasField("ManualManufacturingMultiplier") && Result->TryGetField("ManualManufacturingMultiplier")->Type == EJson::Number)
	{
		Recipe.ManualManufacturingMultiplier = Result->TryGetField("ManualManufacturingMultiplier")->AsNumber();
	}
	if (Result->HasField("VariablePowerConsumptionFactor") && Result->TryGetField("VariablePowerConsumptionFactor")->Type == EJson::Number)
	{
		Recipe.VariablePowerConsumptionFactor = Result->TryGetField("VariablePowerConsumptionFactor")->AsNumber();
	}
	if (Result->HasField("VariablePowerConsumptionConstant") && Result->TryGetField("VariablePowerConsumptionConstant")->Type == EJson::Number)
	{
		Recipe.VariablePowerConsumptionConstant = Result->TryGetField("VariablePowerConsumptionConstant")->AsNumber();
	}
	if (Result->HasField("ManufacturingMenuPriority") && Result->TryGetField("ManufacturingMenuPriority")->Type == EJson::Number)
	{
		Recipe.ManufacturingMenuPriority = Result->TryGetField("ManufacturingMenuPriority")->AsNumber();
	}
	
	if(Result->HasField("Ingredients") && Result->TryGetField("Ingredients")->Type == EJson::Array)
	{
		for(auto i : Result->TryGetField("Ingredients")->AsArray())
		{
			if(i->Type == EJson::Object)
			{
				const auto Obj = i->AsObject();
				const bool HasItem = Obj->HasField("Item");
				const bool HasAmount = Obj->HasField("Amount");
				if (HasItem && HasAmount)
				{
					const TSharedPtr<FJsonValue> Item = Obj->TryGetField("Item");
					const TSharedPtr<FJsonValue> Amount = Obj->TryGetField("Amount");

					if (Item->Type == EJson::String && Amount->Type == EJson::Number)
					{
						Recipe.Ingredients.Add(Item->AsString(), Amount->AsNumber());
					}
					else
					{
						if (Item->Type != EJson::String)
						{
							UE_LOG(LogTemp, Error, TEXT("Invalid Type on Ingredient. Expected String"));
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Invalid Type on Amount! Expected Number"));
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("No Item or Amount In Json %s"), *String);
				}
			}
		}
	}
	else
	{
		Recipe.ClearIngredients = false;
	}
	if(Result->HasField("Products") && Result->TryGetField("Products")->Type == EJson::Array)
	{
		for(auto i : Result->TryGetField("Products")->AsArray())
		{
			if (i->Type == EJson::Object)
			{
				const auto Obj = i->AsObject();
				const bool HasItem = Obj->HasField("Item");
				const bool HasAmount = Obj->HasField("Amount");
				if (HasItem && HasAmount)
				{
					const TSharedPtr<FJsonValue> Item = Obj->TryGetField("Item");
					const TSharedPtr<FJsonValue> Amount = Obj->TryGetField("Amount");

					if (Item->Type == EJson::String && Amount->Type == EJson::Number)
					{
						Recipe.Products.Add(Item->AsString(), Amount->AsNumber());
					}
					else
					{
						if (Item->Type != EJson::String)
						{
							UE_LOG(LogTemp, Error, TEXT("Invalid Type on Product. Expected String"));
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Invalid Type on Amount! Expected Number"));
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("No Item or Amount In Json %s"), *String);
				}
			}
		}
	}
	else
	{
		Recipe.ClearProducts = false;
	}
	if(Result->HasField("ProducedIn") && Result->TryGetField("ProducedIn")->Type == EJson::Array)
	{
		for(auto i : Result->TryGetField("ProducedIn")->AsArray())
		{
			if(i->Type == EJson::String)
			{
				Recipe.BuildIn.Add(i->AsString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid Type on ProducedIn! Expected String"));
			}
		}
	}
	else
	{	
		Recipe.ClearBuilders = false;	
	}
	if (Result->HasField("UnlockedBy") && Result->TryGetField("UnlockedBy")->Type == EJson::Array)
	{
		for (auto i : Result->TryGetField("UnlockedBy")->AsArray())
		{
			if (i->Type == EJson::String)
			{
				Recipe.UnlockedBy.Add(i->AsString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid Type on UnlockedBy! Expected String"));
			}
		}
	}

	if (Result->HasField("ClearIngredients") && Result->TryGetField("ClearIngredients")->Type == EJson::Boolean)
	{
		Recipe.ClearIngredients = Result->TryGetField("ClearIngredients")->AsBool();
	}

	if (Result->HasField("ClearProducts") && Result->TryGetField("ClearProducts")->Type == EJson::Boolean)
	{
		Recipe.ClearProducts = Result->TryGetField("ClearProducts")->AsBool();
	}

	if (Result->HasField("ClearBuilders") && Result->TryGetField("ClearBuilders")->Type == EJson::Boolean)
	{
		Recipe.ClearBuilders = Result->TryGetField("ClearBuilders")->AsBool();
	}
	
	return Recipe;
}

FString UContentLib_RecipesSubsystem::SerializeRecipe(const TSubclassOf<UFGRecipe> Recipe)
{
	if (!Recipe)
		return "";
	const auto CDO = Cast<UFGRecipe>(Recipe->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto Override = MakeShared<FJsonValueBoolean>(CDO->mDisplayNameOverride);
	const auto Cat = MakeShared<FJsonValueString>(CDO->mOverriddenCategory->GetPathName());
	const auto ManufacturingDuration = MakeShared<FJsonValueNumber>(CDO->mManufactoringDuration);
	const auto mVariablePowerConsumptionFactor = MakeShared<FJsonValueNumber>(CDO->mVariablePowerConsumptionFactor);
	const auto mVariablePowerConsumptionConstant = MakeShared<FJsonValueNumber>(CDO->mVariablePowerConsumptionConstant);

	TArray< TSharedPtr<FJsonValue>> Ingredients;
	TArray< TSharedPtr<FJsonValue>> Products;
	TArray< TSharedPtr<FJsonValue>> ProducedIn; 
	for(auto i : CDO->mIngredients)
	{
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.ItemClass->GetName()));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Amount));
		Ingredients.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	for(auto i : CDO->mProduct)
	{
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.ItemClass->GetName()));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Amount));
		Products.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	for(auto i : CDO->mProducedIn)
	{
		auto IngObj = MakeShared<FJsonObject>();
		ProducedIn.Add(MakeShared<FJsonValueString>(i.GetAssetName()));
	}
	const auto Ing = MakeShared<FJsonValueArray>(Ingredients);
	const auto Prod = MakeShared<FJsonValueArray>(Products);
	const auto Producer = MakeShared<FJsonValueArray>(ProducedIn);

	Obj->Values.Add("Name",Name);
	Obj->Values.Add("OverrideName", Override);
	Obj->Values.Add("Category", Cat);
	Obj->Values.Add("Ingredients",Ing);
	Obj->Values.Add("Products",Prod);
	Obj->Values.Add("ProducedIn",Producer);
	Obj->Values.Add("ManufacturingDuration", ManufacturingDuration);
	Obj->Values.Add("VariablePowerConsumptionFactor", mVariablePowerConsumptionFactor);
	Obj->Values.Add("VariablePowerConsumptionConstant", mVariablePowerConsumptionConstant);
	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FString UContentLib_RecipesSubsystem::SerializeCLRecipe(FContentLib_Recipe Recipe)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(Recipe.Name);
	const auto Override = MakeShared<FJsonValueBoolean>(static_cast<bool>(Recipe.OverrideName));
	const auto Cat = MakeShared<FJsonValueString>(Recipe.Category);
	const auto ManufacturingDuration = MakeShared<FJsonValueNumber>(Recipe.ManufacturingDuration);
	const auto mVariablePowerConsumptionFactor = MakeShared<FJsonValueNumber>(Recipe.VariablePowerConsumptionFactor);
	const auto mVariablePowerConsumptionConstant = MakeShared<FJsonValueNumber>(Recipe.VariablePowerConsumptionConstant);

	
	TArray< TSharedPtr<FJsonValue>> Ingredients;
	TArray< TSharedPtr<FJsonValue>> Products;
	TArray< TSharedPtr<FJsonValue>> ProducedIn; 
	for(auto i : Recipe.Ingredients)
	{
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.Key));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Value));
		Ingredients.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	for(auto i : Recipe.Products)
	{
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.Key));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Value));
		Products.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	for(auto i : Recipe.BuildIn)
	{
		auto IngObj = MakeShared<FJsonObject>();
		ProducedIn.Add(MakeShared<FJsonValueString>(i));
	}
	const auto Ing = MakeShared<FJsonValueArray>(Ingredients);
	const auto Prod = MakeShared<FJsonValueArray>(Products);
	const auto Producer = MakeShared<FJsonValueArray>(ProducedIn);
	const auto ClearIngredients = MakeShared<FJsonValueBoolean>(Recipe.ClearIngredients);
	const auto ClearProducts = MakeShared<FJsonValueBoolean>(Recipe.ClearProducts);
	const auto ClearBuilders = MakeShared<FJsonValueBoolean>(Recipe.ClearBuilders);



	
	Obj->Values.Add("Name",Name);
	Obj->Values.Add("OverrideName", Override);
	Obj->Values.Add("Category", Cat);
	Obj->Values.Add("Ingredients",Ing);
	Obj->Values.Add("Products",Prod);
	Obj->Values.Add("ProducedIn",Producer);
	Obj->Values.Add("ManufacturingDuration", ManufacturingDuration);
	Obj->Values.Add("VariablePowerConsumptionFactor", mVariablePowerConsumptionFactor);
	Obj->Values.Add("VariablePowerConsumptionConstant", mVariablePowerConsumptionConstant);
	Obj->Values.Add("ClearIngredients", ClearIngredients);
	Obj->Values.Add("ClearProducts", ClearProducts);
	Obj->Values.Add("ClearBuilders", ClearBuilders);

	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
};

TSubclassOf<UObject> UContentLib_RecipesSubsystem::CreateContentLibRecipe(FString Name, TSubclassOf<UObject> Class)
{
	if (Name == "" || FindObject<UClass>(ANY_PACKAGE, *Name, false) || FindObject<UClass>(ANY_PACKAGE, *Name.Append("_C"), false))
		return nullptr;
	const EClassFlags ParamsClassFlags = CLASS_Native | CLASS_MatchedSerializers;
	UClass* ParentClass = Class;
	//Code below is taken from GetPrivateStaticClassBody
	//Allocate memory from ObjectAllocator for class object and call class constructor directly
	UClass* ConstructedClassObject = (UClass*)GUObjectAllocator.AllocateUObject(sizeof(UDynamicClass), alignof(UDynamicClass), true);
	::new (ConstructedClassObject)UDynamicClass(
		EC_StaticConstructor,
		*Name,
		ParentClass->GetStructureSize(),
		ParentClass->GetMinAlignment(),
		CLASS_Intrinsic,
		CASTCLASS_None,
		UObject::StaticConfigName(),
		EObjectFlags(RF_Public | RF_Standalone | RF_Transient | RF_MarkAsNative | RF_MarkAsRootSet),
		ParentClass->ClassConstructor,
		ParentClass->ClassVTableHelperCtorCaller,
		ParentClass->ClassAddReferencedObjects, nullptr);

	//Set super structure and ClassWithin (they are required prior to registering)
	FCppClassTypeInfoStatic TypeInfoStatic = { false };
	ConstructedClassObject->SetSuperStruct(ParentClass);
	ConstructedClassObject->ClassWithin = UObject::StaticClass();
	ConstructedClassObject->SetCppTypeInfoStatic(&TypeInfoStatic);
#if WITH_EDITOR
	//Field with cpp type info only exists in editor, in shipping SetCppTypeInfoStatic is empty
	ConstructedClassObject->SetCppTypeInfoStatic(&TypeInfoStatic);
#endif
	//Register pending object, apply class flags, set static type info and link it
	ConstructedClassObject->RegisterDependencies();

	ConstructedClassObject->DeferredRegister(UDynamicClass::StaticClass(), TEXT("/ContentLib_Recipes/"), *Name);

	//Mark class as Constructed and perform linking
	ConstructedClassObject->ClassFlags |= (EClassFlags)(ParamsClassFlags | CLASS_Constructed);
	ConstructedClassObject->AssembleReferenceTokenStream(true);
	ConstructedClassObject->StaticLink();

	//Make sure default class object is initialized
	ConstructedClassObject->GetDefaultObject();
	return ConstructedClassObject;
}
