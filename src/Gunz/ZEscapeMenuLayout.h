#pragma once
#include "ZUiLayout.h"

struct ZEscapeMenuLayout
{
    int Width, Height, Header, Padding, Row, Gap, GroupGap;
    explicit ZEscapeMenuLayout(int screenWidth, int screenHeight)
    {
        const float scale = ZUiLayout::Scale(screenWidth, screenHeight);
        Width = int(360 * scale);
        Header = int(80 * scale);
        Padding = int(20 * scale);
        Row = int(40 * scale);
        Gap = int(8 * scale);
        GroupGap = int(10 * scale);
        Height = Header + 6 * Row + 5 * Gap + 2 * GroupGap + Padding;
    }
    int RowY(int index) const
    {
        return Header + index * (Row + Gap) +
            (index > 2 ? GroupGap : 0) + (index > 4 ? GroupGap : 0);
    }
};
