#ifndef UNREALSDK_UNREAL_STRUCTS_FGAMEDATAHANDLE_H
#define UNREALSDK_UNREAL_STRUCTS_FGAMEDATAHANDLE_H

#include "unrealsdk/pch.h"

namespace unrealsdk::unreal {

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()

struct FGameDataHandle {
    uint8_t raw[0x18]{};
};

static_assert(sizeof(FGameDataHandle) == 0x18);

UNREALSDK_UNREAL_STRUCT_PADDING_POP()

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FGAMEDATAHANDLE_H */
