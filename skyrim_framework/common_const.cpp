#include "skyrim_framework\common_const.h"

namespace Common{
	const char* FormContainer::modNames[FormArrayCount] { "Skyrim.esm" };
	const UInt32 FormContainer::FormArrayEndingIndex[FormArrayCount] { FormCount };

	std::array<UInt32, FormContainer::FormCount> FormContainer::Forms = { 0x581F4, 0x79355, 0x58F69, 0x6F993, 0x800E4, 0x5ACE4, 0x79358, 0x1E717, 0x1E718, 0x1E719, 0x1E71A, 0x1E71B, 0x1E71C, 0x1E71D, 0x1E71E, 0x1E71F, 0x10fd61, 0x10fd62, 0x1711d, 0x1711f, 0xbcd2a, 0x7935e, 0x6D930, 0xed06d };
}
