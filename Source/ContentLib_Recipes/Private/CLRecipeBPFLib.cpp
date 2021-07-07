


#include "CLRecipeBPFLib.h"


#include "FGSchematic.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Unlocks/FGUnlockRecipe.h"

FContentLib_Recipe UCLRecipeBPFLib::GenerateFromString(FString String)
{
	if(!String.StartsWith("{") || !String.EndsWith("}"))
		return FContentLib_Recipe();
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	FContentLib_Recipe Recipe;
	if(Result->HasField("Name") && Result->TryGetField("Name")->Type == EJson::String)
	{
		Recipe.Name = Result->TryGetField("Name")->AsString();
	}
	if(Result->HasField("Ingredients") && Result->TryGetField("Ingredients")->Type == EJson::Array)
	{
		for(auto i : Result->TryGetField("Ingredients")->AsArray())
		{
			if(i->Type == EJson::Object)
			{
				if(i->AsObject()->HasField("Item") && i->AsObject()->HasField("Amount"))
				{
					if(i->AsObject()->TryGetField("Item")->Type == EJson::String && i->AsObject()->TryGetField("Amount")->Type == EJson::Number)
					{
						Recipe.Ingredients.Add(i->AsObject()->TryGetField("Item")->AsString(),i->AsObject()->TryGetField("Amount")->AsNumber());
					}
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
			if(i->Type == EJson::Object)
			{
				if(i->AsObject()->HasField("Item") && i->AsObject()->HasField("Amount"))
				{
					if(i->AsObject()->TryGetField("Item")->Type == EJson::String && i->AsObject()->TryGetField("Amount")->Type == EJson::Number)
					{
						Recipe.Products.Add(i->AsObject()->TryGetField("Item")->AsString(),i->AsObject()->TryGetField("Amount")->AsNumber());
					}
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


FString UCLRecipeBPFLib::SerializeRecipe(const TSubclassOf<UFGRecipe> Recipe)
{
	const auto CDO = Cast<UFGRecipe>(Recipe->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());

	
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
	const auto Prod = MakeShared<FJsonValueArray>(Ingredients);
	const auto Producer = MakeShared<FJsonValueArray>(ProducedIn);

	Obj->Values.Add("Name",Name);
	Obj->Values.Add("Ingredients",Ing);
	Obj->Values.Add("Products",Prod);
	Obj->Values.Add("ProducedIn",Producer);
	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FString UCLRecipeBPFLib::SerializeCLRecipe(FContentLib_Recipe Recipe)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(Recipe.Name);
	const auto ManufacturingDuration = MakeShared<FJsonValueNumber>(Recipe.ManufacturingDuration);
	const auto mVariablePowerConsumptionFactor = MakeShared<FJsonValueNumber>(Recipe.mVariablePowerConsumptionFactor);
	const auto mVariablePowerConsumptionConstant = MakeShared<FJsonValueNumber>(Recipe.mVariablePowerConsumptionConstant);

	
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
	const auto Prod = MakeShared<FJsonValueArray>(Ingredients);
	const auto Producer = MakeShared<FJsonValueArray>(ProducedIn);
	
	Obj->Values.Add("Name",Name);
	Obj->Values.Add("Ingredients",Ing);
	Obj->Values.Add("Products",Prod);
	Obj->Values.Add("ProducedIn",Producer);
	Obj->Values.Add("ManufacturingDuration", ManufacturingDuration);
	Obj->Values.Add("mVariablePowerConsumptionFactor", mVariablePowerConsumptionFactor);
	Obj->Values.Add("mVariablePowerConsumptionConstant", mVariablePowerConsumptionConstant);

	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
};

void UCLRecipeBPFLib::WriteStringToFile(FString Path, FString resultString, bool Relative) { 
#if WITH_EDITOR 
	FFileHelper::SaveStringToFile(resultString, Relative ? *(FPaths::ProjectDir() + Path) : *Path);
	
#else
	const FString AbsoluteRootPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	const FString AbsolutePath = AbsoluteRootPath + TEXT("Mods/") + Path;
	if (!AbsolutePath.Contains(TEXT("..")))
	{
		FFileHelper::SaveStringToFile(resultString, *AbsolutePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Absolute or escaping Paths are not allowed in Runtime"));
}
#endif
}

bool UCLRecipeBPFLib::LoadStringFromFile(FString& String, FString Path, bool Relative) {
#if WITH_EDITOR 
	return FFileHelper::LoadFileToString(String, Relative ? *(FPaths::ProjectDir() + Path) : *Path);
#else
	const FString AbsoluteRootPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	const FString AbsolutePath = FPaths::ConvertRelativePathToFull(Path);
	if (AbsolutePath.StartsWith(AbsoluteRootPath))
	{
		return FFileHelper::LoadFileToString(String, *AbsolutePath);
	}
	else
	{
		return false;
		UE_LOG(LogTemp, Error, TEXT("Absolute or escaping Paths are not allowed in Runtime"));
	}
#endif
}

bool UCLRecipeBPFLib::GetDirectoriesInPath(const FString& FullPathOfBaseDir, TArray<FString>& DirsOut, const FString& NotContainsStr , bool Recursive, const FString& ContainsStr)
{
	FString Str;

	auto FilenamesVisitor = MakeDirectoryVisitor(
		[&](const TCHAR* FilenameOrDirectory, bool bIsDirectory)
		{
			if (bIsDirectory)
			{
				//Using a Contains Filter?
				if (ContainsStr != "")
				{
					Str = FPaths::GetCleanFilename(FilenameOrDirectory);
					//Only if Directory Contains Str		
					if (Str.Contains(ContainsStr))
					{
						if (Recursive) DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
						else DirsOut.Push(Str);
					}

				}
				else if (NotContainsStr != "") {
					if (!Str.Contains(NotContainsStr))
					{
						if (Recursive) DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
						else DirsOut.Push(Str);
					}
				}
				//Get ALL Directories!
				else
				{
					//Just the Directory
					Str = FPaths::GetCleanFilename(FilenameOrDirectory);

					if (Recursive) DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
					else DirsOut.Push(Str);
				}
			}
			return true;
		}
	);
	if (Recursive)
	{
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*FullPathOfBaseDir, FilenamesVisitor);
	}
	else
	{
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*FullPathOfBaseDir, FilenamesVisitor);
	}
}

void UCLRecipeBPFLib::String_Sort(UPARAM(ref) TArray <FString>& Array_To_Sort, bool Descending, bool FilterToUnique,  TArray <FString>& Sorted_Array)
{
	if (FilterToUnique)
	{
		for (auto i : Array_To_Sort)
		{
			if (FilterToUnique)
			{
				if (!Sorted_Array.Contains(i))
				{
					Sorted_Array.Add(i); 
				}
			}
		}
	}
	Sorted_Array.Sort();               // Sort array using built in function (sorts A-Z)

	if (Descending == true)
	{
		TArray <FString> NewArray;      // Define "temp" holding array
		int x = Sorted_Array.Num() - 1;

		while (x > -1)
		{
			NewArray.Add(Sorted_Array[x]); // loop through A-Z sorted array and remove element from back and place it in beginning of "temp" array
			--x;
		}

		Sorted_Array = NewArray;   // Set reference array to "temp" array order, array is now Z-A
	}
}

bool UCLRecipeBPFLib::GetFilesInPath(const FString& FullPathOfBaseDir, TArray<FString>& FilenamesOut, bool Recursive, const FString& FilterByExtension)
{
	//Format File Extension, remove the "." if present
	const FString FileExt = FilterByExtension.Replace(TEXT("."), TEXT("")).ToLower();

	FString Str;
	auto FilenamesVisitor = MakeDirectoryVisitor(
		[&](const TCHAR* FilenameOrDirectory, bool bIsDirectory)
		{
			//Files
			if (!bIsDirectory)
			{
				//Filter by Extension
				if (FileExt != "")
				{
					Str = FPaths::GetCleanFilename(FilenameOrDirectory);

					//Filter by Extension
					if (FPaths::GetExtension(Str).ToLower() == FileExt)
					{
						if (Recursive)
						{
							FilenamesOut.Push(FilenameOrDirectory); //need whole path for recursive
						}
						else
						{
							FilenamesOut.Push(Str);
						}
					}
				}

				//Include All Filenames!
				else
				{
					//Just the Directory
					Str = FPaths::GetCleanFilename(FilenameOrDirectory);

					if (Recursive)
					{
						FilenamesOut.Push(FilenameOrDirectory); //need whole path for recursive
					}
					else
					{
						FilenamesOut.Push(Str);
					}
				}
			}
			return true;
		}
	);
	if (Recursive)
	{
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*FullPathOfBaseDir, FilenamesVisitor);
	}
	else
	{
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*FullPathOfBaseDir, FilenamesVisitor);
	}
}