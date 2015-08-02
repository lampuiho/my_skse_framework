#pragma once
#include <functional>
#include "Utilities.h"
#include <unordered_map>
#include "skse/GameObjects.h"
#include "skse/GameExtraData.h"

namespace papyrusObjectReference
{
	void Container_for_each(TESObjectREFR* obj, std::function<void(TESForm*, UInt32)> del);

	struct ContainerForEach{
		TESObjectREFR* pObj;
		std::function<void(TESForm*, UInt32)> pDel;
		std::unordered_map<TESForm*, UInt32> entries;

		ContainerForEach(TESObjectREFR* obj, std::function<void(TESForm*, UInt32)> del);
		void operator()();
		bool Accept(TESContainer::Entry* pEntry);
		bool Accept(InventoryEntryData* data);
	};
}