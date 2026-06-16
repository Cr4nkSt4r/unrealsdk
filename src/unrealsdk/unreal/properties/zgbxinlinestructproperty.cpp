#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/properties/zgbxinlinestructproperty.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fgbxinlinestruct.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(ZGbxInlineStructProperty,
                                    UNREALSDK_ZGBXINLINESTRUCTPROPERTY_FIELDS);

PropTraits<ZGbxInlineStructProperty>::Value PropTraits<ZGbxInlineStructProperty>::get(
    const ZGbxInlineStructProperty* /*prop*/,
    uintptr_t addr,
    const UnrealPointer<void>& /*parent*/) {
    return *reinterpret_cast<FGbxInlineStruct*>(addr);
}

void PropTraits<ZGbxInlineStructProperty>::set(const ZGbxInlineStructProperty* /*prop*/,
                                               uintptr_t addr,
                                               const Value& value) {
    auto* const dest = reinterpret_cast<FGbxInlineStruct*>(addr);
    if (dest == &value) {
        return;
    }

    FGbxInlineStruct copied{};
    try {
        gbx_inline_struct_copy(copied, value);
    } catch (...) {
        gbx_inline_struct_reset(copied);
        throw;
    }

    gbx_inline_struct_reset(*dest);
    *dest = copied;
}

void PropTraits<ZGbxInlineStructProperty>::destroy(const ZGbxInlineStructProperty* /*prop*/,
                                                   uintptr_t addr) {
    gbx_inline_struct_reset(*reinterpret_cast<FGbxInlineStruct*>(addr));
}

}  // namespace unrealsdk::unreal
