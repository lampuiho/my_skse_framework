#include "MyPapyrusObjectReference.h"
#include "skse/GameRTTI.h"
#include <algorithm>

namespace papyrusObjectReference
{
	void Container_for_each(TESObjectREFR* obj, std::function<void(TESForm*, UInt32)> del){
		ContainerForEach iterator(obj, del);
		return iterator();
	}

	ContainerForEach::ContainerForEach(TESObjectREFR* obj, std::function<void(TESForm*, UInt32)> del) : pObj(obj), pDel(del){}

	void ContainerForEach::operator()(){
		TESContainer* pContainer = DYNAMIC_CAST(pObj->baseForm, TESForm, TESContainer);

		if (pContainer){
			entries.reserve(pContainer->numEntries);
			pContainer->Visit(*this);
		}

		ExtraContainerChanges* pXContainerChanges = static_cast<ExtraContainerChanges*>(pObj->extraData.GetByType(kExtraData_ContainerChanges));
		if (pXContainerChanges && pXContainerChanges->data->objList)
			pXContainerChanges->data->objList->Visit(*this);
	}

	bool ContainerForEach::Accept(TESContainer::Entry* pEntry){
		if (pEntry)
			if (pEntry->form && !DYNAMIC_CAST(pEntry->form, TESForm, TESLevItem))
				entries.insert({ pEntry->form, pEntry->count });
		return true;
	}

	bool ContainerForEach::Accept(InventoryEntryData* pEntry){
		if (pEntry)
			if (pEntry->type && !DYNAMIC_CAST(pEntry->type, TESForm, TESLevItem)){
				SInt32 count = pEntry->countDelta;
				auto it = entries.find(pEntry->type);
				if (it != entries.end())
					count += it->second;
				if (count > 0)
					if (pDel)
						pDel(pEntry->type, count);
#ifdef _DEBUG
					else
						std::cout << "ContainerForEach:: pDel is empty. Cannot call delegate function." << std::endl;
#endif
			}
		return true;
	}
}
