#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/properties/zgamedatahandleproperty.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"

namespace unrealsdk::unreal {

namespace {

constexpr auto CACHE_OFFSET = size_t{0x10};

}  // namespace

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(ZGameDataHandleProperty,
                                    UNREALSDK_ZGAMEDATAHANDLEPROPERTY_FIELDS);

PropTraits<ZGameDataHandleProperty>::Value PropTraits<ZGameDataHandleProperty>::get(
    const ZGameDataHandleProperty*,
    uintptr_t addr,
    const UnrealPointer<void>&) {
    return *reinterpret_cast<FGameDataHandle*>(addr);
}

void PropTraits<ZGameDataHandleProperty>::set(const ZGameDataHandleProperty*,
                                              uintptr_t addr,
                                              const Value& value) {
    auto* const out = reinterpret_cast<FGameDataHandle*>(addr);
    *out = value;

    // Preserve identity fields while preventing wrapper to wrapper assignment from
    // retaining another owner's cache.
    std::fill(std::begin(out->raw) + CACHE_OFFSET, std::end(out->raw), 0);
}

void PropTraits<ZGameDataHandleProperty>::destroy(const ZGameDataHandleProperty*,
                                                  uintptr_t) {}

}  // namespace unrealsdk::unreal
