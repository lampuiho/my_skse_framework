#include "skse/PapyrusVM.h"
#include "skse/PapyrusNativeFunctions.h"
#include "skse/GameReferences.h"
#include "skse/GameExtraData.h"
#include <vector>
#include <unordered_map>
#include <memory>

DEFINE_MEMBER_FN_LONG(InventoryEntryData, IsQuestItem, bool, 0x004759B0);

namespace LevelersTower{
	class ContainerInfo{
	protected:
		struct RemovalInfo{
			TESForm* form;
			SInt32 count;
			UInt8 equipState;
			RemovalInfo(UInt32 c, UInt8 e) : count(c), equipState(e) {}
			RemovalInfo(TESForm* _form, UInt32 c, UInt8 e) : form(_form), count(c), equipState(e) {}
		};
		typedef std::unordered_map<TESForm*, RemovalInfo> ContainerInfoMap;
	private:
		ContainerInfoMap m_map;
	public:
		typedef std::vector<RemovalInfo> ContainerInfoVector;
		void Init(TESObjectREFR*container);
		virtual bool Accept(InventoryEntryData* data);
		bool Accept(TESContainer::Entry* pEntry);
		bool ValidateEntry(TESForm* form);
		template <typename Op>
		void Visit(Op& op) const;
		UInt32 Count() const;
	protected:
		ContainerInfoVector m_vec;
		void registerRemovalInfo(TESForm* form, SInt32 count, UInt8 equipState);
	};

	class ContainerRemovalInfoWithException : public ContainerInfo{
		std::vector<TESForm*> exceptionList;
	public:
		void Accept(ContainerInfoVector::const_iterator it);
		bool Accept(InventoryEntryData* data);
		ContainerRemovalInfoWithException(TESObjectREFR* exceptionListContainer);
	};

	class ContainerCleaner{
		TESObjectREFR* m_akActionRef;
		TESObjectREFR* m_targetContainer;
	protected:
		std::unique_ptr<ContainerInfo> m_info;
		void RemoveItem(TESForm* akBaseItem, SInt32 aiCount, TESObjectREFR* targetContainer);
	public:
		~ContainerCleaner(){
			m_info.reset(nullptr);
		}
		ContainerCleaner(TESObjectREFR* akActionRef, TESObjectREFR* targetContainer);
		void afterConstruction();
		virtual void Accept(ContainerInfo::ContainerInfoVector::const_iterator pEntry);
	};

	class ContainerSorter : public ContainerCleaner
	{
		BGSListForm* m_containerRefList;
	protected:
		virtual void SortItem(TESForm* akBaseItem, SInt32 aiCount);
		void RemoveItem(TESForm* akBaseItem, SInt32 aiCount, SInt32 containerCode);
	public:
		virtual void Accept(ContainerInfo::ContainerInfoVector::const_iterator pEntry);
		ContainerSorter(TESObjectREFR* akActionRef, BGSListForm* containerRefList);
	};

	class ActorSorter : public ContainerSorter
	{
		Actor* m_akActionRef_Actor;
		void EquipItem(TESForm* akBaseItem, SInt8 equipState);
	public:
		void ActorSorter::Accept(ContainerInfo::ContainerInfoVector::const_iterator pEntry);
		ActorSorter(Actor* akActionRef, BGSListForm* containerRefList);
	};

	class IngredSorter : public ContainerSorter{
		void SortItem(TESForm* akBaseItem, SInt32 aiCount);
	public:
		IngredSorter(TESObjectREFR* akActionRef, BGSListForm* containerRefList);
	};

	bool SortIngred(StaticFunctionTag* thisObjRef, TESObjectREFR* akActionRef);
	bool SortAllItems(StaticFunctionTag* thisObjRef, TESObjectREFR* akActionRef);
	void TakeAllIngred(StaticFunctionTag* thisObjRef, TESObjectREFR* akActionRef);
	void TakeEverything(StaticFunctionTag* thisObjRef, TESObjectREFR* akActionRef);
	bool MyRegisterFuncs(VMClassRegistry* registry);
}