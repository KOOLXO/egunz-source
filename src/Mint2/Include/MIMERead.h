#pragma once

#include <cstddef>

namespace MIMERead
{
// Getter uses the IMM byte-count contract. Reserve a terminator and reject
// partial/failed reads rather than dispatching stale or truncated text.
template <typename Char, std::size_t N, typename Context, typename Getter>
bool String(Context context, unsigned long kind, Char (&output)[N], Getter get)
{
    output[0] = 0;
    Char buffer[N] = {};
    const auto bytes = get(context, kind, nullptr, 0);
    if (bytes <= 0 || bytes % sizeof(Char) != 0 ||
        static_cast<std::size_t>(bytes) > (N - 1) * sizeof(Char))
        return false;
    const auto read = get(context, kind, buffer, static_cast<unsigned long>(bytes));
    if (read != bytes)
        return false;
    const auto length = static_cast<std::size_t>(read) / sizeof(Char);
    for (std::size_t i = 0; i < length; ++i)
    {
        if (buffer[i] == 0)
            return false;
    }
    for (std::size_t i = 0; i < length; ++i)
        output[i] = buffer[i];
    output[length] = 0;
    return true;
}

template <std::size_t N>
bool ValidUTF16(const wchar_t (&text)[N])
{
    for (std::size_t i = 0; i < N; ++i)
    {
        const auto ch = static_cast<unsigned int>(text[i]);
        if (ch == 0) return true;
        if (ch >= 0xD800 && ch <= 0xDBFF)
        {
            if (++i == N) return false;
            const auto low = static_cast<unsigned int>(text[i]);
            if (low < 0xDC00 || low > 0xDFFF) return false;
        }
        else if (ch >= 0xDC00 && ch <= 0xDFFF)
            return false;
    }
    return false;
}
}
