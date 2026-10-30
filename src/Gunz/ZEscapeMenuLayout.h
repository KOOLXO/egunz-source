#pragma once
#include <algorithm>

struct ZEscapeMenuLayout
{
    int Width, Height, Header, Padding, Row, Gap, GroupGap, FontPoints;
    explicit ZEscapeMenuLayout(int screenWidth, int screenHeight)
    {
        const float scale = (std::max)(.85f, (std::min)(1.5f,
            (std::min)(screenWidth / 1920.f, screenHeight / 1080.f)));
        Width = int(360 * scale);
        Header = int(80 * scale);
        Padding = int(20 * scale);
        Row = int(40 * scale);
        Gap = int(8 * scale);
        GroupGap = int(10 * scale);
        FontPoints = int(12 * scale + .5f);
        Height = Header + 6 * Row + 5 * Gap + 2 * GroupGap + Padding;
    }
    int RowY(int index) const
    {
        return Header + index * (Row + Gap) +
            (index > 2 ? GroupGap : 0) + (index > 4 ? GroupGap : 0);
    }
};
