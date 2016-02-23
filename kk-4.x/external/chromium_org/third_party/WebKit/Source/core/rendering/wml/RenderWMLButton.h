/*
 * Copyright (C) 2005 Apple Computer
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

#ifndef RenderWMLButton_h
#define RenderWMLButton_h

#if ENABLE(WML)
#include "core/rendering/RenderButton.h"

namespace WebCore {

class RenderTextFragment;

// RenderWMLButtons are just like normal flexboxes except that they will generate an anonymous block child.
// For inputs, they will also generate an anonymous RenderText and keep its style and content up
// to date as the button changes.
class RenderWMLButton : public RenderButton {
public:
    explicit RenderWMLButton(Element*);

    virtual const char* renderName() const { return "RenderWMLButton"; }
    virtual bool isRenderWMLButton() const { return true; }

    virtual void updateFromElement();
    void setText(const String&);

private:
    RenderTextFragment* m_buttonText;

};
} // namespace WebCore

#endif

#endif // RenderWMLButton_h
