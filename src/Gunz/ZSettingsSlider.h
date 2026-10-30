#pragma once

#include "MSlider.h"

// Option sliders use button clicks for one predictable step per press.
// The base scrollbar only polls held buttons; quick clicks can be missed.
class ZSettingsSlider : public MSlider
{
protected:
    void OnRun() override {}
    bool OnCommand(MWidget* sender, const char* message) override
    {
        if (!MWidget::IsMsg(message, MBTN_CLK_MSG) ||
            (sender != m_pUp && sender != m_pDown))
            return false;
        const int value = GetValue();
        const int next = sender == m_pUp ?
            (value > GetMin() ? value - 1 : GetMin()) :
            (value < GetMax() ? value + 1 : GetMax());
        if (next != value)
        {
            SetValue(next);
            if (auto* listener = GetListener())
                listener->OnCommand(this, MLIST_VALUE_CHANGED);
        }
        return true;
    }
public:
    ZSettingsSlider(const char* name, MWidget* parent, MListener* listener)
        : MSlider(name, parent, listener) {}
};
