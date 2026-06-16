#ifndef UNREALSDK_UNREAL_STRUCTS_FINLINESTRUCT_H
#define UNREALSDK_UNREAL_STRUCTS_FINLINESTRUCT_H

#include "unrealsdk/pch.h"

namespace unrealsdk::unreal {

class UScriptStruct;

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()

struct FGbxInlineStruct {
    struct Control {
        uintptr_t* vftable;
        uint32_t refs_a;
        uint32_t refs_b;
        uint32_t possibly_offset_to_data;
        uint32_t unknown;
    };

    void* instance{};
    Control* control{};
    uint64_t flags{};
};

UNREALSDK_UNREAL_STRUCT_PADDING_POP()

bool gbx_inline_struct_assign_construct(FGbxInlineStruct& value,
                                        UScriptStruct* type,
                                        bool preserve_existing_payload = false,
                                        bool copy_matching_fields = false);
void gbx_inline_struct_copy(FGbxInlineStruct& dest, const FGbxInlineStruct& src);
void gbx_inline_struct_reset(FGbxInlineStruct& value) noexcept;
bool gbx_inline_struct_native_helpers_available(void) noexcept;

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_FINLINESTRUCT_H */
