//
// Created by abina on 7/18/2025.
//

#include "TinyEngineUI.h"


void TinyEngineUI::render(TinyEngineFrameBuffer _framebuffer)
{
}

void TinyEngineUI::update(double frameTime)
{
}

TinyEngineUI::~TinyEngineUI()
{
}

void TinyEngineUIText::render(TinyEngineFrameBuffer _framebuffer)
{
    // TinyEngineUI::render(_framebuffer);
    _framebuffer.draw_string(m_text.c_str(), m_x, m_y, m_color);
}

void TinyEngineUIText::update(double frameTime)
{
    TinyEngineUI::update(frameTime);
}

TinyEngineUIText::~TinyEngineUIText()
{
}

void TinyEngineUIBlinkingTextBox::render(TinyEngineFrameBuffer _framebuffer)
{
    TinyEngineUI::render(_framebuffer);

    _framebuffer.draw_filled_rectangle(m_x, m_y, m_width, m_height, m_bg_color);
    _framebuffer.draw_outline_rectangle(m_x, m_y, m_width, m_height, m_fg_color);
    _framebuffer.draw_string(m_text.c_str(),
                             m_x + (m_width / 2) - (m_text.length() / 2 * 7),
                             // get the half of the text. maybe make this cached value to make it more efficent.
                             m_y + m_height / 2 - (8 / 2), // the 8 is fixed here since the chars are 8 bits tall.
                             m_fg_color);
}

void TinyEngineUIBlinkingTextBox::update(double frameTime)
{
    TinyEngineUI::update(frameTime);
    m_local_time += frameTime * 10;
    if (m_local_time >= m_blink_delay)
    {
        m_local_time = 0;
        std::swap(m_fg_color, m_bg_color);
    }
}

TinyEngineUIBlinkingTextBox::~TinyEngineUIBlinkingTextBox()
{
}

void TinyEngineUITextBox::render(TinyEngineFrameBuffer _framebuffer)
{
    TinyEngineUI::render(_framebuffer);
    _framebuffer.draw_outline_rectangle(m_x, m_y, m_width, m_height, m_outline_color);

    for (auto text : m_texts)
    {
        _framebuffer.draw_string(text.second.get_text().c_str(), m_x + text.second.get_m_x(), m_y + text.second.get_m_y(),
                                 text.second.get_m_color());
    }
}

void TinyEngineUITextBox::update(double frameTime)
{
    TinyEngineUI::update(frameTime);
}

TinyEngineUITextBox::~TinyEngineUITextBox()
{
}
