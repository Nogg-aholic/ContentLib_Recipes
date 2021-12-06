


#include "CLItemBPFLib.h"
#include "BPFContentLib.h"
#include "FGWorldSettings.h"
#include "Serialization/JsonSerializer.h"




FContentLib_VisualKit::FContentLib_VisualKit()
{
	FluidColor = FColor();
	GasColor = FColor();
}

UTexture2D* FContentLib_VisualKit::GetSmallIcon() const
{
	return LoadObject<UTexture2D>(nullptr, *SmallIcon);
}

UTexture2D* FContentLib_VisualKit::GetBigIcon() const
{
	return LoadObject<UTexture2D>(nullptr, *BigIcon);
}

UStaticMesh* FContentLib_VisualKit::GetMesh() const
{
	return LoadObject<UStaticMesh>(nullptr, *Mesh);
}

TSharedRef<FJsonObject> FContentLib_VisualKit::GetAsJsonObject(const TSubclassOf<UFGItemDescriptor> Item)
{
	const auto CDO = Cast<UFGItemDescriptor>(Item->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	const auto Mesh = MakeShared<FJsonValueString>(UFGItemDescriptor::GetItemMesh(Item)->GetPathName());
	const auto BigIcon = MakeShared<FJsonValueString>(UFGItemDescriptor::GetBigIcon(Item)->GetPathName());
	const auto SmallIcon = MakeShared<FJsonValueString>(UFGItemDescriptor::GetSmallIcon(Item)->GetPathName());
	auto Color = MakeShared<FJsonObject>();
	const auto fluid = UFGItemDescriptor::GetFluidColor(Item);
	const auto FluidColor_R = MakeShared<FJsonValueNumber>(fluid.R);
	const auto FluidColor_G = MakeShared<FJsonValueNumber>(fluid.G);
	 const auto FluidColor_B = MakeShared<FJsonValueNumber>(fluid.B);
	const auto FluidColor_A = MakeShared<FJsonValueNumber>(fluid.A);
	Color->Values.Add("r", FluidColor_R);
	Color->Values.Add("g", FluidColor_G);
	Color->Values.Add("b", FluidColor_B);
	Color->Values.Add("a", FluidColor_A);

	auto ColorGas = MakeShared<FJsonObject>();
	const auto gasColor = UFGItemDescriptor::GetGasColor(Item);
	const auto GasColor_R = MakeShared<FJsonValueNumber>(gasColor.R);
	const auto GasColor_G = MakeShared<FJsonValueNumber>(gasColor.G);
	const auto GasColor_B = MakeShared<FJsonValueNumber>(gasColor.B);
	const auto GasColor_A = MakeShared<FJsonValueNumber>(gasColor.A);
	ColorGas->Values.Add("r", GasColor_R);
	ColorGas->Values.Add("g", GasColor_G);
	ColorGas->Values.Add("b", GasColor_B);
	ColorGas->Values.Add("a", GasColor_A);

	Obj->Values.Add("Mesh", Mesh);
	Obj->Values.Add("BigIcon", BigIcon);
	Obj->Values.Add("SmallIcon", SmallIcon);
	Obj->Values.Add("FluidColor", MakeShared<FJsonValueObject>(Color));
	Obj->Values.Add("GasColor", MakeShared<FJsonValueObject>(ColorGas));

	return Obj;
}





FContentLib_ResourceItem::FContentLib_ResourceItem(): CollectSpeedMultiplier(-1)
{
	PingColor = FColor(0, 0, 0, 0);
}

TSharedRef<FJsonObject> FContentLib_ResourceItem::GetResourceAsJsonObject(TSubclassOf<UFGResourceDescriptor> Item)
{
	const auto CDO = Cast<UFGResourceDescriptor>(Item->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	auto Color = MakeShared<FJsonObject>();
	const auto pingColor = UFGResourceDescriptor::GetPingColor(Item);
	const auto PingColor_R = MakeShared<FJsonValueNumber>(pingColor.R);
	const auto PingColor_G = MakeShared<FJsonValueNumber>(pingColor.G);
	const auto PingColor_B = MakeShared<FJsonValueNumber>(pingColor.B);
	const auto PingColor_A = MakeShared<FJsonValueNumber>(pingColor.A);
	Color->Values.Add("r", PingColor_R);
	Color->Values.Add("g", PingColor_G);
	Color->Values.Add("b", PingColor_B);
	Color->Values.Add("a", PingColor_A);
	const auto CollectSpeedMultiplier = MakeShared<FJsonValueNumber>(UFGResourceDescriptor::GetCollectSpeedMultiplier(Item));

	Obj->Values.Add("PingColor", MakeShared<FJsonValueObject>(Color));
	Obj->Values.Add("CollectSpeedMultiplier", CollectSpeedMultiplier);

	return Obj;
}


FContentLib_NuclearFuelItem::FContentLib_NuclearFuelItem(): AmountOfWaste(-1)
{
}

TSharedRef<FJsonObject> FContentLib_NuclearFuelItem::GetNuclearFuelAsJsonObject(
	TSubclassOf<UFGItemDescriptorNuclearFuel> Item)
{
	const auto CDO = Cast<UFGItemDescriptorNuclearFuel>(Item->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();

	const auto SpentFuelClass = MakeShared<FJsonValueString>(CDO->GetSpentFuelClass(Item)->GetPathName());

	const auto AmountOfWaste = MakeShared<FJsonValueNumber>(CDO->GetAmountWasteCreated(Item));

	Obj->Values.Add("SpentFuelClass", SpentFuelClass);
	Obj->Values.Add("AmountOfWaste", AmountOfWaste);
	return Obj;

}


FContentLib_Item::FContentLib_Item(): Form(EResourceForm::RF_INVALID), StackSize(EStackSize::SS_LAST_ENUM), EnergyValue(-1),
                                      RadioactiveDecay(-1),
                                      CanBeDiscarded(-1), RememberPickUp(-1),
                                      ResourceSinkPoints(-1),
                                      FuelWasteItem()
{
}

FString UCLItemBPFLib::GenerateStringFromCLItem(FContentLib_Item Item)
{
	const auto CDO = Item;
	const auto Obj = MakeShared<FJsonObject>();
	FString FormString = "Invalid";
	if (CDO.Form == EResourceForm::RF_SOLID)
	{
		FormString = "Solid";
	}
	else if (CDO.Form == EResourceForm::RF_LIQUID)
	{
		FormString = "Liquid";
	}
	else if (CDO.Form == EResourceForm::RF_GAS)
	{
		FormString = "Gas";
	}
	else if (CDO.Form == EResourceForm::RF_HEAT)
	{
		FormString = "Heat";
	}
	FString SizeString = "Invalid";

	if (CDO.StackSize == EStackSize::SS_ONE)
	{
		SizeString = "One";
	}
	else if (CDO.StackSize == EStackSize::SS_SMALL)
	{
		SizeString = "Small";
	}
	else if (CDO.StackSize == EStackSize::SS_MEDIUM)
	{
		SizeString = "Medium";
	}
	else if (CDO.StackSize == EStackSize::SS_BIG)
	{
		SizeString = "Big";
	}
	else if (CDO.StackSize == EStackSize::SS_HUGE)
	{
		SizeString = "Huge";
	}
	else if (CDO.StackSize == EStackSize::SS_FLUID)
	{
		SizeString = "Liquid";
	}
	else if (CDO.StackSize == EStackSize::SS_LAST_ENUM)
	{
		SizeString = "Invalid";
	}

	const auto Form = MakeShared<FJsonValueString>(FormString);
	const auto Size = MakeShared<FJsonValueString>(SizeString);
	Obj->Values.Add("Form", Form);
	Obj->Values.Add("StackSize", Size);

	if (CDO.Name != "")
	{
		const auto Name = MakeShared<FJsonValueString>(CDO.Name);
		Obj->Values.Add("Name", Name);
	}

	if (CDO.NameShort != "")
	{
		const auto NameShort = MakeShared<FJsonValueString>(CDO.NameShort);
		Obj->Values.Add("NameShort", NameShort);
	}

	if (CDO.Description != "")
	{
		const auto Description = MakeShared<FJsonValueString>(CDO.Description);
		Obj->Values.Add("Description", Description);
	}
	if (CDO.Category != "")
	{
		const auto ItemCategory = MakeShared<FJsonValueString>(CDO.Category);
		Obj->Values.Add("Category", ItemCategory);
	}
	if (CDO.VisualKit != "")
	{
		const auto ItemCategory = MakeShared<FJsonValueString>(CDO.VisualKit);
		Obj->Values.Add("VisualKit", ItemCategory);
	}

	if (CDO.EnergyValue != -1)
	{
		const auto EnergyValue = MakeShared<FJsonValueNumber>(CDO.EnergyValue);
		Obj->Values.Add("EnergyValue", EnergyValue);
	}

	if (CDO.RadioactiveDecay != -1)
	{
		const auto RadioactiveDecay = MakeShared<FJsonValueNumber>(CDO.RadioactiveDecay);
		Obj->Values.Add("RadioactiveDecay", RadioactiveDecay);
	}

	if (CDO.CanBeDiscarded != -1)
	{
		const auto CanBeDiscarded = MakeShared<FJsonValueBoolean>(static_cast<bool>(CDO.CanBeDiscarded));
		Obj->Values.Add("CanBeDiscarded", CanBeDiscarded);
	}
	if (CDO.RememberPickUp != -1)
	{
		const auto RememberPickUp = MakeShared<FJsonValueBoolean>(static_cast<bool>(CDO.RememberPickUp));
		Obj->Values.Add("RememberPickUp", RememberPickUp);
	}

	if (CDO.ResourceSinkPoints != -1)
	{
		const auto ResourceSinkPoints = MakeShared<FJsonValueNumber>(CDO.ResourceSinkPoints);
		Obj->Values.Add("ResourceSinkPoints", ResourceSinkPoints);
	}

	if (CDO.ResourceItem.CollectSpeedMultiplier != -1 || CDO.ResourceItem.PingColor != FColor(0,0,0,0))
	{
		const auto Objx = MakeShared<FJsonObject>();

		if (CDO.ResourceItem.PingColor != FColor(0, 0, 0, 0))
		{
			auto Color = MakeShared<FJsonObject>();
			const auto PingColor_R = MakeShared<FJsonValueNumber>(CDO.ResourceItem.PingColor.R);
			const auto PingColor_G = MakeShared<FJsonValueNumber>(CDO.ResourceItem.PingColor.G);
			const auto PingColor_B = MakeShared<FJsonValueNumber>(CDO.ResourceItem.PingColor.B);
			const auto PingColor_A = MakeShared<FJsonValueNumber>(CDO.ResourceItem.PingColor.A);
			Color->Values.Add("r", PingColor_R);
			Color->Values.Add("g", PingColor_G);
			Color->Values.Add("b", PingColor_B);
			Color->Values.Add("a", PingColor_A);
			Objx->Values.Add("PingColor", MakeShared<FJsonValueObject>(Color));

		}
		if (CDO.ResourceItem.CollectSpeedMultiplier != -1)
		{
			const auto CollectSpeedMultiplier = MakeShared<FJsonValueNumber>(CDO.ResourceItem.CollectSpeedMultiplier);
			Objx->Values.Add("CollectSpeedMultiplier", CollectSpeedMultiplier);

		}

		Obj->Values.Add("ResourceItem", MakeShared<FJsonValueObject>(Objx));
	}

	if (CDO.FuelWasteItem.SpentFuelClass != "" || CDO.FuelWasteItem.AmountOfWaste != -1)
	{
		const auto Objx = MakeShared<FJsonObject>();

		if (CDO.FuelWasteItem.SpentFuelClass != "")
		{
			const auto SpentFuelClass = MakeShared<FJsonValueString>(CDO.FuelWasteItem.SpentFuelClass);
			Objx->Values.Add("SpentFuelClass", SpentFuelClass);
		}
		if (CDO.FuelWasteItem.AmountOfWaste != -1)
		{
			const auto AmountOfWaste = MakeShared<FJsonValueNumber>(CDO.FuelWasteItem.AmountOfWaste);
			Obj->Values.Add("AmountOfWaste", AmountOfWaste);

		}
		Obj->Values.Add("FuelWasteItem", MakeShared<FJsonValueObject>(Objx));
	}

	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
		wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FString UCLItemBPFLib::GenerateFromDescriptorClass(TSubclassOf<UFGItemDescriptor> Item)
{
if (!Item)
		return "";

	const auto CDO = Cast<UFGItemDescriptor>(Item->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	FString FormString = "Invalid";
	if (CDO->mForm == EResourceForm::RF_SOLID)
	{
		FormString = "Solid";
	}
	else if (CDO->mForm == EResourceForm::RF_LIQUID)
	{
		FormString = "Liquid";
	}
	else if (CDO->mForm == EResourceForm::RF_GAS)
	{
		FormString = "Gas";
	}
	else if (CDO->mForm == EResourceForm::RF_HEAT)
	{
		FormString = "Heat";
	}
	FString SizeString = "Invalid";

	if (CDO->mStackSize == EStackSize::SS_ONE)
	{
		SizeString = "One";
	}
	else if (CDO->mStackSize == EStackSize::SS_SMALL)
	{
		SizeString = "Small";
	}
	else if (CDO->mStackSize == EStackSize::SS_MEDIUM)
	{
		SizeString = "Medium";
	}
	else if (CDO->mStackSize == EStackSize::SS_BIG)
	{
		SizeString = "Big";
	}
	else if (CDO->mStackSize == EStackSize::SS_HUGE)
	{
		SizeString = "Huge";
	}
	else if (CDO->mStackSize == EStackSize::SS_FLUID)
	{
		SizeString = "Liquid";
	}
	else if (CDO->mStackSize == EStackSize::SS_LAST_ENUM)
	{
		SizeString = "Invalid";
	}
	
	const auto Form = MakeShared<FJsonValueString>(FormString);
	const auto Size = MakeShared<FJsonValueString>(SizeString);
	Obj->Values.Add("Form", Form);
	Obj->Values.Add("StackSize", Size);

	if (CDO->mDisplayName.ToString() != "")
	{
		const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
		Obj->Values.Add("Name", Name);
	}

	if (CDO->mAbbreviatedDisplayName.ToString() != "")
	{
		const auto NameShort = MakeShared<FJsonValueString>(CDO->mAbbreviatedDisplayName.ToString());
		Obj->Values.Add("NameShort", NameShort);
	}

	if (CDO->mDescription.ToString() != "")
	{
		const auto Description = MakeShared<FJsonValueString>(CDO->mDescription.ToString());
		Obj->Values.Add("Description", Description);
	}
	if (CDO->mItemCategory)
	{
		const auto ItemCategory = MakeShared<FJsonValueString>(CDO->mItemCategory->GetPathName());
		Obj->Values.Add("Category", ItemCategory);
	}

	const auto VisualKit = MakeShared<FJsonValueObject>(FContentLib_VisualKit::GetAsJsonObject(Item));
	Obj->Values.Add("VisualKit", VisualKit);

	if (CDO->mEnergyValue != 0)
	{
		const auto EnergyValue = MakeShared<FJsonValueNumber>(CDO->mEnergyValue);
		Obj->Values.Add("EnergyValue", EnergyValue);
	}

	if (CDO->mRadioactiveDecay != 0)
	{
		const auto RadioactiveDecay = MakeShared<FJsonValueNumber>(CDO->mRadioactiveDecay);
		Obj->Values.Add("RadioactiveDecay", RadioactiveDecay);
	}

	if (CDO->mCanBeDiscarded)
	{
		const auto CanBeDiscarded = MakeShared<FJsonValueBoolean>(CDO->mCanBeDiscarded);
		Obj->Values.Add("CanBeDiscarded", CanBeDiscarded);
	}
	if (CDO->mRememberPickUp)
	{
		const auto RememberPickUp = MakeShared<FJsonValueBoolean>(CDO->mRememberPickUp);
		Obj->Values.Add("RememberPickUp", RememberPickUp);
	}

	if (CDO->mResourceSinkPoints != 0)
	{
		const auto ResourceSinkPoints = MakeShared<FJsonValueNumber>(CDO->mResourceSinkPoints);
		Obj->Values.Add("ResourceSinkPoints", ResourceSinkPoints);
	}

	if (Item->IsChildOf(UFGResourceDescriptor::StaticClass()))
	{
		TSubclassOf<UFGResourceDescriptor> Resource = Item;
		const auto ResourceItem = MakeShared<FJsonValueObject>(FContentLib_ResourceItem::GetResourceAsJsonObject(Resource));
		Obj->Values.Add("ResourceItem", ResourceItem);
	}

	if (Item->IsChildOf(UFGItemDescriptorNuclearFuel::StaticClass()))
	{
		TSubclassOf<UFGItemDescriptorNuclearFuel> Resource = Item;
		const auto FuelWasteItem = MakeShared<FJsonValueObject>(FContentLib_NuclearFuelItem::GetNuclearFuelAsJsonObject(Resource));
		Obj->Values.Add("FuelWasteItem", FuelWasteItem);
	}

	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
		wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;}

FContentLib_Item UCLItemBPFLib::GenerateCLItemFromString(FString String)
{
if (String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
		return FContentLib_Item();

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid())
		return FContentLib_Item();

	auto Item = FContentLib_Item();
	if (Result->HasField("Form") && Result->TryGetField("Form")->Type == EJson::String)
	{
		const FString CS = Result->TryGetField("Form")->AsString();
		if (CS.Equals("Solid", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_SOLID;
		}
		else if (CS.Equals("Liquid", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_LIQUID;
		}
		else if (CS.Equals("Gas", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_GAS;
		}
		else if (CS.Equals("Heat", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_HEAT;
		}
	}


	if (Result->HasField("StackSize") && Result->TryGetField("StackSize")->Type == EJson::String)
	{
		const FString CS = Result->TryGetField("StackSize")->AsString();
		if (CS.Equals("One", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_ONE;
		}
		else if (CS.Equals("Small", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_SMALL;
		}
		else if (CS.Equals("Medium", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_MEDIUM;
		}
		else if (CS.Equals("Big", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_BIG;
		}
		else if (CS.Equals("Huge", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_HUGE;
		}
		else if (CS.Equals("Liquid", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_FLUID;
		}
	}
	UBPFContentLib::SetStringFieldWithLog(Item.Name, "Name", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.NameShort, "NameShort", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.Description, "Description", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.Category, "Category", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.VisualKit, "VisualKit", Result);
	UBPFContentLib::SetFloatFieldWithLog(Item.EnergyValue, "EnergyValue", Result);
	UBPFContentLib::SetFloatFieldWithLog(Item.RadioactiveDecay, "RadioactiveDecay", Result);
	UBPFContentLib::SetIntegerFieldWithLog(Item.CanBeDiscarded, "CanBeDiscarded", Result);
	UBPFContentLib::SetIntegerFieldWithLog(Item.RememberPickUp, "RememberPickUp", Result);
	UBPFContentLib::SetIntegerFieldWithLog(Item.ResourceSinkPoints, "ResourceSinkPoints", Result);


	if (Result->HasField("ResourceItem") && Result->TryGetField("ResourceItem")->Type == EJson::Object)
	{
		const auto OBJ = Result->TryGetField("ResourceItem")->AsObject();
		UBPFContentLib::SetLinearColorFieldWithLog(Item.ResourceItem.PingColor, "PingColor", OBJ);
		UBPFContentLib::SetFloatFieldWithLog(Item.ResourceItem.CollectSpeedMultiplier, "CollectSpeedMultiplier", OBJ);
	}

	if (Result->HasField("FuelWasteItem") && Result->TryGetField("FuelWasteItem")->Type == EJson::Object)
	{
		const auto OBJ = Result->TryGetField("FuelWasteItem")->AsObject();
		UBPFContentLib::SetStringFieldWithLog(Item.FuelWasteItem.SpentFuelClass, "SpentFuelClass", Result);
		UBPFContentLib::SetIntegerFieldWithLog(Item.FuelWasteItem.AmountOfWaste, "AmountOfWaste", Result);
	}
	return Item;}

FContentLib_VisualKit UCLItemBPFLib::GenerateKitFromString(FString String)
{
	if (String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
	{
		if (String == "")
			UE_LOG(LogTemp, Error, TEXT("Empty String  %s"), *String)
		else if (!String.StartsWith("{"))
			UE_LOG(LogTemp, Error, TEXT("String doesnt start with '{' %s"), *String)
		else if (!String.EndsWith("}"))
			UE_LOG(LogTemp, Error, TEXT("String doesnt end with '}'  %s"), *String);

		return FContentLib_VisualKit();
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Json ! %s"), *String);
		return FContentLib_VisualKit();
	}

	FContentLib_VisualKit Item;

	UBPFContentLib::SetStringFieldWithLog(Item.Mesh, "Mesh", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.BigIcon, "BigIcon", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.SmallIcon, "SmallIcon", Result);
	UBPFContentLib::SetColorFieldWithLog(Item.FluidColor, "FluidColor", Result);
	UBPFContentLib::SetColorFieldWithLog(Item.GasColor, "GasColor", Result);

	return Item;
}

FString UCLItemBPFLib::GenerateKitFromClass(TSubclassOf<UFGItemDescriptor> Item)
{
	if (!Item)
	return "";

	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
        wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(FContentLib_VisualKit::GetAsJsonObject(Item), JsonWriter);
	return Write;

}

FString UCLItemBPFLib::GetVisualKitAsString(FContentLib_VisualKit Kit)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto Mesh = MakeShared<FJsonValueString>(Kit.Mesh);
	const auto BigIcon = MakeShared<FJsonValueString>(Kit.BigIcon);
	const auto SmallIcon = MakeShared<FJsonValueString>(Kit.SmallIcon);
	auto Color = MakeShared<FJsonObject>();
	const auto FluidColor_R = MakeShared<FJsonValueNumber>(Kit.FluidColor.R);
	const auto FluidColor_G = MakeShared<FJsonValueNumber>(Kit.FluidColor.G);
	const auto FluidColor_B = MakeShared<FJsonValueNumber>(Kit.FluidColor.B);
	const auto FluidColor_A = MakeShared<FJsonValueNumber>(Kit.FluidColor.A);
	Color->Values.Add("r", FluidColor_R);
	Color->Values.Add("g", FluidColor_G);
	Color->Values.Add("b", FluidColor_B);
	Color->Values.Add("a", FluidColor_A);

	auto ColorGas = MakeShared<FJsonObject>();
	const auto GasColor_R = MakeShared<FJsonValueNumber>(Kit.GasColor.R);
	const auto GasColor_G = MakeShared<FJsonValueNumber>(Kit.GasColor.G);
	const auto GasColor_B = MakeShared<FJsonValueNumber>(Kit.GasColor.B);
	const auto GasColor_A = MakeShared<FJsonValueNumber>(Kit.GasColor.A);
	ColorGas->Values.Add("r", GasColor_R);
	ColorGas->Values.Add("g", GasColor_G);
	ColorGas->Values.Add("b", GasColor_B);
	ColorGas->Values.Add("a", GasColor_A);

	Obj->Values.Add("Mesh", Mesh);
	Obj->Values.Add("BigIcon", BigIcon);
	Obj->Values.Add("SmallIcon", SmallIcon);
	Obj->Values.Add("FluidColor", MakeShared<FJsonValueObject>(Color));
	Obj->Values.Add("GasColor", MakeShared<FJsonValueObject>(ColorGas));
	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

void UCLItemBPFLib::InitItemFromStruct(const TSubclassOf<UFGItemDescriptor> Item, FContentLib_Item ItemStruct,UContentLibSubsystem * Subsystem)
{
	if(!Item)
		return;
	UFGItemDescriptor* CDO = Item.GetDefaultObject();

	if (ItemStruct.Form != EResourceForm::RF_INVALID)
	{
		CDO->mForm = ItemStruct.Form;
	}

	if(ItemStruct.StackSize != EStackSize::SS_LAST_ENUM)
	{
		CDO->mStackSize = ItemStruct.StackSize;
	}
	if (ItemStruct.Name != "")
	{
		CDO->mDisplayName = FText::FromString(ItemStruct.Name);
		CDO->mUseDisplayNameAndDescription = true;
	}
	if (ItemStruct.NameShort != "")
	{
		CDO->mAbbreviatedDisplayName = FText::FromString(ItemStruct.NameShort);
	}
	if (ItemStruct.Description != "")
	{
		CDO->mDescription = FText::FromString(ItemStruct.Description);
	}
	if (ItemStruct.Category != "")
	{
		TSubclassOf<UFGItemCategory> Out = UBPFContentLib::SetCategoryWithLoad(*ItemStruct.Category, Subsystem, false);

		if (Out)
		{
			CDO->mItemCategory = Out;
		}
	}
	if (ItemStruct.VisualKit != "")
	{
		if (ItemStruct.VisualKit.Contains("{") && ItemStruct.VisualKit.Contains("}"))
		{
			FContentLib_VisualKit Kit = GenerateKitFromString(ItemStruct.VisualKit);
			ApplyVisualKitToItem(Subsystem, Kit, Item);
		}
		else
		{
			if (Subsystem->ImportedVisualKits.Contains(ItemStruct.VisualKit))
				ApplyVisualKitToItem(Subsystem, *Subsystem->ImportedVisualKits.Find(ItemStruct.VisualKit), Item);
			else if (Subsystem->VisualKits.Contains(ItemStruct.VisualKit))
				ApplyVisualKitToItem(Subsystem, *Subsystem->VisualKits.Find(ItemStruct.VisualKit), Item);
		}
	}

	if (ItemStruct.EnergyValue != -1)
	{
		CDO->mEnergyValue = ItemStruct.EnergyValue;
	}

	if (ItemStruct.RadioactiveDecay != -1)
	{
		CDO->mRadioactiveDecay = ItemStruct.RadioactiveDecay;
	}

	if (ItemStruct.CanBeDiscarded != -1)
	{
		CDO->mCanBeDiscarded = static_cast<bool>(ItemStruct.CanBeDiscarded);
	}

	if (ItemStruct.RememberPickUp != -1)
	{
		CDO->mRememberPickUp = static_cast<bool>(ItemStruct.RememberPickUp);
	}

	if (ItemStruct.ResourceSinkPoints != -1)
	{
		CDO->mResourceSinkPoints = ItemStruct.ResourceSinkPoints;
	}

	if (Item->IsChildOf(UFGResourceDescriptor::StaticClass()))
	{
		if (ItemStruct.ResourceItem.PingColor != FColor(0, 0, 0, 0))
			Cast<UFGResourceDescriptor>(CDO)->mPingColor = ItemStruct.ResourceItem.PingColor;
		if (ItemStruct.ResourceItem.CollectSpeedMultiplier != -1)
		{
			Cast<UFGResourceDescriptor>(CDO)->mCollectSpeedMultiplier = ItemStruct.ResourceItem.CollectSpeedMultiplier;
		}
	}
	if (Item->IsChildOf(UFGItemDescriptorNuclearFuel::StaticClass()))
	{
		if (ItemStruct.FuelWasteItem.SpentFuelClass != "")
		{
			if (ItemStruct.FuelWasteItem.SpentFuelClass.Contains("/"))
			{
				UClass* Loaded = LoadObject<UClass>(nullptr, *ItemStruct.FuelWasteItem.SpentFuelClass);
				if (Loaded && Loaded->IsChildOf(UFGItemCategory::StaticClass())) {}

					//UFGItemDescriptorNuclearFuel::GetSpentFuelClass(Item) = dynamic_cast<UFGItemDescriptor>(Loaded);
			}
			else
			{
				for (auto e : Subsystem->mItems)
				{
					if (UBPFContentLib::StringCompareItem(e->GetName(),ItemStruct.FuelWasteItem.SpentFuelClass,"Desc","_C"))
					{
						//UFGItemDescriptorNuclearFuel::GetSpentFuelClass(Item) = e;
						break;
					}
				}
			}
		}
		if (ItemStruct.FuelWasteItem.AmountOfWaste != -1)
		{
			//UFGItemDescriptorNuclearFuel::GetAmountWasteCreated(ItemStruct) = ItemStruct.FuelWasteItem.AmountOfWaste;
		}
	}
}



FString UCLItemBPFLib::GenerateFromNuclearFuelClass(TSubclassOf<UFGItemDescriptor> Item)
{
	if (!Item)
		return "";

	if (Item->IsChildOf(UFGItemDescriptorNuclearFuel::StaticClass()))
	{
		TSubclassOf<UFGItemDescriptorNuclearFuel> Resource = Item;
		FString Write;
		const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
            wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
		FJsonSerializer::Serialize(FContentLib_NuclearFuelItem::GetNuclearFuelAsJsonObject(Resource), JsonWriter);
		return Write;
	}
	else
		return "";
}



FString UCLItemBPFLib::GenerateResourceFromClass(TSubclassOf<UFGItemDescriptor> Item)

{
	if (!Item)
		return "";

	if (Item->IsChildOf(UFGResourceDescriptor::StaticClass()))
	{
		TSubclassOf<UFGResourceDescriptor> Resource = Item;
		FString Write;
		const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<
            wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
		FJsonSerializer::Serialize(FContentLib_ResourceItem::GetResourceAsJsonObject(Resource), JsonWriter);
		return Write;
	}
	else
		return "";
}

void UCLItemBPFLib::ApplyVisualKitToItem(UContentLibSubsystem* Subsystem, FContentLib_VisualKit Kit, TSubclassOf<UFGItemDescriptor> Item)
{
	if(!Item)
		return;
	auto* Obj = Item.GetDefaultObject();
	if (Kit.GetMesh())
		Obj->mConveyorMesh = Kit.GetMesh();


	if(Subsystem)
	{
		if(Subsystem->Icons.Contains(Kit.BigIcon))
		{
			Obj->mPersistentBigIcon = *Subsystem->Icons.Find(Kit.BigIcon);
		}
		else
		{
			if (Kit.GetBigIcon())
				Obj->mPersistentBigIcon = Kit.GetBigIcon();
		}
	}
	else
	{
		if (Kit.GetBigIcon())
			Obj->mPersistentBigIcon = Kit.GetBigIcon();
	}
	
	if(Subsystem)
	{
		if(Subsystem->Icons.Contains(Kit.SmallIcon))
		{
			Obj->mSmallIcon = *Subsystem->Icons.Find(Kit.SmallIcon);
		}
		else
		{
			if (Kit.GetSmallIcon())
				Obj->mSmallIcon = Kit.GetSmallIcon();
		}
	}
	else
	{
		if (Kit.GetSmallIcon())
			Obj->mSmallIcon = Kit.GetSmallIcon();
	}

	if (Kit.FluidColor != FColor(0, 0, 0, 0))
	{
		Obj->mFluidColor = Kit.FluidColor;
	}

	if (Kit.GasColor != FColor(0, 0, 0, 0))
	{
		Obj->mGasColor = Kit.GasColor;
	}
}


