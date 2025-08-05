//
// Created by abina on 7/18/2025.
//

#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "TinyEngineRenderableI.h"

class TinyEngineUI : public TinyEngineRenderableI
{
protected:
    uint16_t m_x, m_y, m_x_half, m_y_half, m_width, m_height;

public:
    TinyEngineUI(uint16_t m_x, uint16_t m_y, uint16_t m_width, uint16_t m_height)
        : m_x(m_x),
          m_y(m_y),
          m_width(m_width),
          m_height(m_height)
    {
        m_x_half = m_x / 2;
        m_y_half = m_y / 2;
    }

    virtual void render(TinyEngineFrameBuffer _framebuffer) override;
    virtual void update(double frameTime) override;
    ~TinyEngineUI() override;

    uint16_t get_m_x() const
    {
        return m_x;
    }

    uint16_t get_m_y() const
    {
        return m_y;
    }

    uint16_t get_m_x_half() const
    {
        return m_x_half;
    }

    uint16_t get_m_y_half() const
    {
        return m_y_half;
    }

    void set_m_x(uint16_t m_x)
    {
        this->m_x = m_x;
    }

    void set_m_y(uint16_t m_y)
    {
        this->m_y = m_y;
    }

    void set_m_x_half(uint16_t m_x_half)
    {
        this->m_x_half = m_x_half;
    }

    void set_m_y_half(uint16_t m_y_half)
    {
        this->m_y_half = m_y_half;
    }

    void set_m_width(uint16_t m_width)
    {
        this->m_width = m_width;
    }

    void set_m_height(uint16_t m_height)
    {
        this->m_height = m_height;
    }

    uint16_t get_m_width() const
    {
        return m_width;
    }

    uint16_t get_m_height() const
    {
        return m_height;
    }
};

class TinyEngineUIText : public TinyEngineUI
{
protected:
    // vec2 pos = {0, 0};
    std::string m_text;
    uint8_t m_color;

public:
    void render(TinyEngineFrameBuffer _framebuffer) override;
    void update(double frameTime) override;
    ~TinyEngineUIText() override;

    TinyEngineUIText(uint16_t m_x, uint16_t m_y, const std::string& text, uint8_t _color)
        : TinyEngineUI(m_x, m_y, 0, 0),
          m_text(text),
          m_color(_color)
    {
    }

    std::string& get_text()
    {
        return m_text;
    }

    void set_text(const std::string& text)
    {
        this->m_text = text;
    }

    uint8_t get_m_color() const
    {
        return m_color;
    }

    void set_m_color(uint8_t m_color)
    {
        this->m_color = m_color;
    }
};

class TinyEngineUIBlinkingTextBox : public TinyEngineUI
{
protected:
    uint8_t m_bg_color, m_fg_color;
    std::string m_text;
    uint8_t m_blink_delay;
    float m_local_time = 0;

public:
    void render(TinyEngineFrameBuffer _framebuffer) override;
    void update(double frameTime) override;
    ~TinyEngineUIBlinkingTextBox() override;

    TinyEngineUIBlinkingTextBox(uint16_t m_x, uint16_t m_y, uint16_t m_width,
                                uint16_t m_height, uint8_t m_bg_color, uint8_t m_fg_color, const std::string& m_text,
                                uint8_t m_blink_delay)
        : TinyEngineUI(m_x, m_y, m_width, m_height),
          m_bg_color(m_bg_color),
          m_fg_color(m_fg_color),
          m_text(m_text),
          m_blink_delay(m_blink_delay)
    {
    }

    /**
     * Auto calculate the required width for the outline box.
     * @param m_x
     * @param m_y
     * @param m_bg_color
     * @param m_fg_color
     * @param m_text
     * @param m_blink_delay
     */
    // TinyEngineBlinkingBanner(uint16_t _x, uint16_t _y, uint8_t _bg_color, uint8_t _fg_color,
    //                          const std::string& _text,
    //                          uint8_t _blink_delay = 5)
    // {
    //     TinyEngineUI(m_x, m_y, 10, 8);
    //     set_m_width(m_text.length() * 7);
    //
    //     m_bg_color = _bg_color;
    //     m_fg_color = _fg_color;
    //     m_text = _text;
    //     m_blink_delay = _blink_delay;
    // }

    uint8_t get_m_bg_color() const
    {
        return m_bg_color;
    }

    uint8_t get_m_fg_color() const
    {
        return m_fg_color;
    }

    std::string get_m_text() const
    {
        return m_text;
    }

    uint8_t get_m_blink_delay() const
    {
        return m_blink_delay;
    }
};


class TinyEngineUITextBox : public TinyEngineUI
{
protected:
    std::map<std::string, TinyEngineUIText&> m_texts;
    uint8_t m_outline_color = 15;

public:
    void render(TinyEngineFrameBuffer _framebuffer) override;
    void update(double frameTime) override;
    ~TinyEngineUITextBox() override;

    TinyEngineUITextBox(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height) :
        TinyEngineUI(_x, _y, _width,
                     _height)
    {
    }

    TinyEngineUITextBox(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, uint8_t _outline_color) :
        TinyEngineUI(_x, _y, _width,
                     _height), m_outline_color(_outline_color)
    {
    }

    void add_text(std::string _name, TinyEngineUIText& _text)
    {
        // this->m_texts[_name] = _text;
        this->m_texts.insert({_name, _text});
    }

    uint8_t get_m_outline_color() const
    {
        return m_outline_color;
    }

    void set_m_outline_color(uint8_t m_outline_color)
    {
        this->m_outline_color = m_outline_color;
    }
};
