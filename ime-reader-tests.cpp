#include <windows.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include "src/Mint2/Include/MIMERead.h"

int main()
{
    static_assert(sizeof(wchar_t) == 2, "Test Windows UTF-16 storage");
    struct Guard { unsigned before = 0x12345678; wchar_t text[4]; unsigned after = 0x87654321; } guard;
    const wchar_t source[] = {0x54C8, 0xD83D, 0xDE00, 0};
    int calls = 0;
    auto getter = [&](int, unsigned long, void* dest, unsigned long bytes) -> LONG {
        ++calls;
        if (!dest) return 6;
        assert(bytes == 6);
        std::memcpy(dest, source, bytes);
        return 6;
    };
    assert(MIMERead::String(0, 0, guard.text, getter));
    assert(MIMERead::ValidUTF16(guard.text));
    assert(guard.text[3] == 0 && guard.text[0] == 0x54C8 && calls == 2);
    // Equal consecutive commits are delivered independently, without timing.
    assert(MIMERead::String(0, 0, guard.text, getter) && calls == 4);
    for (LONG queried : {-2L, -1L, 0L, 1L, 7L, 8L, 4096L}) {
        calls = 0;
        guard.text[0] = L'X';
        auto invalid = [&](int, unsigned long, void*, unsigned long) -> LONG {
            ++calls; return queried;
        };
        assert(!MIMERead::String(0, 0, guard.text, invalid));
        assert(calls == 1 && guard.text[0] == 0);
    }
    for (LONG returned : {-2L, 0L, 2L, 4L, 8L}) {
        auto race = [&](int, unsigned long, void* dest, unsigned long) -> LONG {
            if (!dest) return 6;
            std::memcpy(dest, source, 6); return returned;
        };
        assert(!MIMERead::String(0, 0, guard.text, race));
        assert(guard.text[0] == 0);
    }
    auto nul = [](int, unsigned long, void* dest, unsigned long) -> LONG {
        if (dest) std::memset(dest, 0, 2);
        return 2;
    };
    assert(!MIMERead::String(0, 0, guard.text, nul));
    wchar_t high[] = {0xD800, 0};
    wchar_t low[] = {0xDC00, 0};
    wchar_t unterminated[] = {L'A', L'B'};
    assert(!MIMERead::ValidUTF16(high) && !MIMERead::ValidUTF16(low));
    assert(!MIMERead::ValidUTF16(unterminated));
    char legacy[4];
    auto ansi = [](int, unsigned long, void* dest, unsigned long) -> LONG {
        if (dest) std::memcpy(dest, "abc", 3); return 3;
    };
    assert(MIMERead::String(0, 0, legacy, ansi) && !std::strcmp(legacy, "abc"));
    assert(guard.before == 0x12345678 && guard.after == 0x87654321);
    std::cout << "PASS: UTF-16/ANSI boundaries, repeated results, errors, size changes, NUL and surrogate validation\n";
}
