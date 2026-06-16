#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/structs/fgbxinlinestruct.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2

#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unrealsdk.h"

using namespace unrealsdk::memory;

namespace unrealsdk::unreal {

namespace {

// Going the easy way and re-using the games internal functions before re-writing them.
using AssignConstructFn = uint8_t (*)(FGbxInlineStruct*, UScriptStruct*, uint8_t, uint8_t);
using CopyConstructFn = FGbxInlineStruct* (*)(FGbxInlineStruct*, const FGbxInlineStruct*);
using ResetReleaseFn = void (*)(FGbxInlineStruct*);

// CURRENTLY ONLY TESTED FOR VERSION 1.6.0, HAVEN'T HAD TIME TO LOOK AT 1.7.0
const constinit Pattern<77> GBX_INLINE_STRUCT_ASSIGN_CONSTRUCT_PATTERN{
    "41 57"              // push r15
    "41 56"              // push r14
    "41 55"              // push r13
    "41 54"              // push r12
    "56"                 // push rsi
    "57"                 // push rdi
    "55"                 // push rbp
    "53"                 // push rbx
    "48 83 EC 48"        // sub rsp, 48
    "44 89 CD"           // mov ebp, r9d
    "44 89 C3"           // mov ebx, r8d
    "48 89 D6"           // mov rsi, rdx
    "49 89 CF"           // mov r15, rcx
    "48 8B 05 ????????"  // mov rax, [Borderlands4.exe+...]
    "48 31 E0"           // xor rax, rsp
    "48 89 44 24 40"     // mov [rsp+40], rax
    "4C 8D 61 10"        // lea r12, [rcx+10]
    "4C 89 E1"           // mov rcx, r12
    "E8 ????????"        // call weak ptr helper
    "34 01"              // xor al, 1
    "08 D8"              // or al, bl
    "0F 84 ????????"     // je preserve/rebuild branch
    "45 31 ED"           // xor r13d, r13d
    "48 85 F6"           // test rsi, rsi
    "0F 84 ????????"     // je reset branch
};

const constinit Pattern<96> GBX_INLINE_STRUCT_COPY_CONSTRUCT_PATTERN{
    "56"                 // push rsi
    "57"                 // push rdi
    "53"                 // push rbx
    "48 83 EC 20"        // sub rsp, 20
    "48 89 CE"           // mov rsi, rcx
    "8B 42 14"           // mov eax, [rdx+14]
    "85 C0"              // test eax, eax
    "74 ??"              // je empty
    "48 89 D7"           // mov rdi, rdx
    "8B 4A 10"           // mov ecx, [rdx+10]
    "85 C9"              // test ecx, ecx
    "78 ??"              // js empty
    "39 0D ????????"     // cmp [GUObjectArray count], ecx
    "7E ??"              // jle empty
    "0F B7 D1"           // movzx edx, cx
    "4C 8B 05 ????????"  // mov r8, [GUObjectArray chunks]
    "C1 E9 10"           // shr ecx, 10
    "48 8D 14 52"        // lea rdx, [rdx+rdx*2]
    "C1 E2 03"           // shl edx, 3
    "49 03 14 C8"        // add rdx, [r8+rcx*8]
    "74 ??"              // je empty
    "39 42 10"           // cmp [rdx+10], eax
    "75 ??"              // jne empty
    "8B 42 08"           // mov eax, [rdx+8]
    "A9 00 00 20 10"     // test eax, 10200000
    "75 ??"              // jne empty
    "48 8B 1A"           // mov rbx, [rdx]
    "48 89 F1"           // mov rcx, rsi
    "48 89 DA"           // mov rdx, rbx
    "45 31 C0"           // xor r8d, r8d
    "45 31 C9"           // xor r9d, r9d
    "E8 ????????"        // call assign/construct
};

const constinit Pattern<85> GBX_INLINE_STRUCT_RESET_RELEASE_PATTERN{
    "56"                         // push rsi
    "57"                         // push rdi
    "48 83 EC 28"                // sub rsp, 28
    "48 89 CE"                   // mov rsi, rcx
    "48 83 39 00"                // cmp qword ptr [rcx], 0
    "75 0F"                      // jne release control
    "48 C7 46 10 00 00 00 00"   // mov qword ptr [rsi+10], 0
    "48 83 C4 28"                // add rsp, 28
    "5F"                         // pop rdi
    "5E"                         // pop rsi
    "C3"                         // ret
    "48 C7 06 00 00 00 00"      // mov qword ptr [rsi], 0
    "48 8B 7E 08"                // mov rdi, [rsi+8]
    "48 85 FF"                   // test rdi, rdi
    "74 E1"                      // je clear weak identity
    "48 C7 46 08 00 00 00 00"   // mov qword ptr [rsi+8], 0
    "F0 FF 4F 08"                // lock dec dword ptr [rdi+8]
    "75 D3"                      // jne clear weak identity
    "48 8B 07"                   // mov rax, [rdi]
    "48 89 F9"                   // mov rcx, rdi
    "FF 10"                      // call qword ptr [rax]
    "F0 FF 4F 0C"                // lock dec dword ptr [rdi+C]
    "75 C5"                      // jne clear weak identity
    "48 8B 07"                   // mov rax, [rdi]
    "48 89 F9"                   // mov rcx, rdi
    "FF 50 10"                   // call qword ptr [rax+10]
    "EB BA"                      // jmp clear weak identity
};

AssignConstructFn assign_construct_helper(void) noexcept {
    static const auto func =
        GBX_INLINE_STRUCT_ASSIGN_CONSTRUCT_PATTERN.sigscan_nullable<AssignConstructFn>();
    return func;
}

CopyConstructFn copy_construct_helper(void) noexcept {
    static const auto func =
        GBX_INLINE_STRUCT_COPY_CONSTRUCT_PATTERN.sigscan_nullable<CopyConstructFn>();
    return func;
}

ResetReleaseFn reset_release_helper(void) noexcept {
    static const auto func =
        GBX_INLINE_STRUCT_RESET_RELEASE_PATTERN.sigscan_nullable<ResetReleaseFn>();
    return func;
}

}  // namespace

bool gbx_inline_struct_assign_construct(FGbxInlineStruct& value,
                                        UScriptStruct* type,
                                        const bool preserve_existing_payload,
                                        const bool copy_matching_fields) {
    const auto func = assign_construct_helper();
    if (func == nullptr) {
        LOG(ERROR, "Could not resolve BL4 FGbxInlineStruct assign/construct helper");
        throw std::runtime_error("FGbxInlineStruct assign/construct helper unavailable");
    }
    return func(&value, type, preserve_existing_payload ? 1 : 0, copy_matching_fields ? 1 : 0)
        != 0;
}

void gbx_inline_struct_copy(FGbxInlineStruct& dest, const FGbxInlineStruct& src) {
    const auto func = copy_construct_helper();
    if (func == nullptr) {
        LOG(ERROR, "Could not resolve BL4 FGbxInlineStruct copy helper");
        throw std::runtime_error("FGbxInlineStruct copy helper unavailable");
    }
    func(&dest, &src);
}

void gbx_inline_struct_reset(FGbxInlineStruct& value) noexcept {
    const auto func = reset_release_helper();
    if (func != nullptr) {
        func(&value);
        return;
    }

    if (value.instance != nullptr || value.control != nullptr || value.flags != 0) {
        LOG(ERROR, "Could not resolve BL4 FGbxInlineStruct reset/release helper");
    }
    value = {};
}

bool gbx_inline_struct_native_helpers_available(void) noexcept {
    return assign_construct_helper() != nullptr && copy_construct_helper() != nullptr
        && reset_release_helper() != nullptr;
}

}  // namespace unrealsdk::unreal

#endif
