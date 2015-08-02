#pragma once
#include "skse\GameEvents.h"
class VMClassRegistry;
struct SKSESerializationInterface;
struct TESLoadGameEvent;
void Serialization_Revert(SKSESerializationInterface * intfc);
void Serialization_Save(SKSESerializationInterface * intfc);
void Serialization_Load(SKSESerializationInterface * intfc);
bool RegisterPapyrus(VMClassRegistry* registry);

template <>
class BSTEventSink<TESLoadGameEvent>
{
public:
	virtual ~BSTEventSink() {}
	virtual	EventResult ReceiveEvent(TESLoadGameEvent * evn, EventDispatcher<TESLoadGameEvent> * dispatcher) = 0;
};
class FormInitialiser : public BSTEventSink <TESLoadGameEvent>{
public:
	EventResult ReceiveEvent(TESLoadGameEvent * evn, EventDispatcher<TESLoadGameEvent> * dispatcher);
};
extern FormInitialiser g_FormInitialiser;

template<typename PluginFormContainer>
bool initialise_forms();