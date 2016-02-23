/**
 * Copyright (C) 2005 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#if ENABLE(WML)
#include "core/rendering/wml/RenderWMLButton.h"

#include "core/dom/Document.h"
#include "core/rendering/RenderTextFragment.h"
#include "core/wml/WMLDoElement.h"
#include "WMLNames.h"

namespace WebCore {

RenderWMLButton::RenderWMLButton(Element* element)
    : RenderButton(element)
    , m_buttonText(0)
{
}

void RenderWMLButton::updateFromElement()
{
    if (node()->hasTagName(WMLNames::doTag)) {
        WMLDoElement* doElement = static_cast<WMLDoElement*>(node());

        String value = doElement->label();
        if (value.isEmpty())
            value = doElement->name();

        setText(value);
    }
}

void RenderWMLButton::setText(const String& str)
{
    if (str.isEmpty()) {
        if (m_buttonText) {
            m_buttonText->destroy();
            m_buttonText = 0;
        }
    } else {
        if (m_buttonText)
            m_buttonText->setText(str.impl());
        else {
            m_buttonText = new RenderTextFragment(&document(), str.impl());
            m_buttonText->setStyle(style());
            addChild(m_buttonText);
        }
    }
}

} // namespace WebCore

#endif
