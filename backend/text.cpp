/**************************************************************************************************
* Copyright (c) 2012 Jørgen Lind
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
* associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute,
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
* NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
* OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
***************************************************************************************************/

#include "text.h"

#include "screen.h"
#include "line.h"
#include <QtQuick/QQuickItem>

#include <QtCore/QDebug>

Text::Text(Line *line)
    : QObject(line)
    , m_line(line)
    , m_style_dirty(true)
    , m_text_dirty(true)
    , m_visible(true)
    , m_visible_old(true)
    , m_forgroundColor(m_line->screen()->defaultForgroundColor())
    , m_backgroundColor(m_line->screen()->defaultBackgroundColor())
{
    connect(screen()->colorPalette(), SIGNAL(changed()), this, SLOT(paletteChanged()));
}

Text::~Text()
{
    emit aboutToBeDestroyed();
}

int Text::index() const
{
    return m_start_index;
}

bool Text::visible() const
{
    return m_visible;
}

void Text::setVisible(bool visible)
{
    m_visible = visible;
}

QString Text::text() const
{
    return m_text;
}

QColor Text::foregroundColor() const
{
    return m_forgroundColor;
}


QColor Text::backgroundColor() const
{
    return m_backgroundColor;
}

void Text::setStringSegment(int start_index, int end_index, bool text_changed)
{
    m_start_index = start_index;
    m_end_index = end_index;

    m_text_dirty = text_changed;
}

void Text::setTextStyle(const TextStyle &style)
{
    m_new_style = style;
    m_style_dirty = true;
}

Screen *Text::screen() const
{
    return m_line->screen();
}

void Text::dispatchEvents()
{
    if (m_style_dirty) {
        m_style_dirty = false;

        bool emit_forground = m_new_style.forground != m_style.forground;
        bool emit_background = m_new_style.background != m_style.background;
        bool emit_text_style = m_new_style.style != m_style.style;

        m_style = m_new_style;
        if (emit_forground) {
            setForgroundColor();
        }
        if (emit_background) {
            setBackgroundColor();
        }
        if (emit_text_style)
            emit textStyleChanged();
    }

    if (m_old_start_index != m_start_index
            || m_text_dirty) {
        m_text_dirty = false;
        m_text = m_line->textLine()->mid(m_start_index, m_end_index + 1 - m_start_index);
        if (m_old_start_index != m_start_index) {
            m_old_start_index = m_start_index;
            emit indexChanged();
        }
        emit textChanged();
    }

    if (m_visible_old != m_visible) {
        m_visible_old = m_visible;
        emit visibleChanged();
    }
}

void Text::paletteChanged()
{
    setBackgroundColor();
    setForgroundColor();
}

void Text::setBackgroundColor()
{
    QColor new_background;
    if (m_style.style & TextStyle::Inverse) {
        new_background = screen()->colorPalette()->color(m_style.forground, false);
    } else {
        new_background = screen()->colorPalette()->color(m_style.background, false);
    }
    if (new_background != m_backgroundColor) {
        m_backgroundColor = new_background;
        emit backgroundColorChanged();
    }
}

void Text::setForgroundColor()
{
    QColor new_forground;
    if (m_style.style & TextStyle::Inverse) {
        new_forground = screen()->colorPalette()->color(m_style.background, false);
    } else {
        new_forground = screen()->colorPalette()->color(m_style.forground, false);
    }
    if (new_forground != m_forgroundColor) {
        m_forgroundColor = new_forground;
        emit forgroundColorChanged();
    }
}
