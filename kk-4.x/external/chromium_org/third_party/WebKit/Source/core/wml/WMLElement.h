/*
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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

#ifndef WMLElement_h
#define WMLElement_h

#if ENABLE(WML)
#include "core/dom/Element.h"
#include "WMLErrorHandling.h"

namespace WebCore {

class WMLElement : public Element {
public:
    static PassRefPtr<WMLElement> create(const QualifiedName& tagName, Document&);

    virtual bool isWMLElement() const { return true; }
    virtual bool isWMLTaskElement() const { return false; }

    void addCSSLengthToStyle(MutableStylePropertySet*, CSSPropertyID, const String& value);

    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;
    virtual bool isPresentationAttribute(const QualifiedName&) const OVERRIDE;
    virtual void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) OVERRIDE;

    virtual String title() const;

    virtual bool rendererIsNeeded(RenderStyle*);
    virtual RenderObject* createRenderer(RenderStyle*);

    virtual void insertedIntoTree(bool);

protected:
    WMLElement(const QualifiedName& tagName, Document&, ConstructionType = CreateWMLElement);

    void applyAlignmentAttributeToStyle(const AtomicString&, MutableStylePropertySet*);
    void applyBorderAttributeToStyle(const AtomicString&, MutableStylePropertySet*);
    unsigned parseBorderWidthAttribute(const AtomicString&) const;

    // Helper function for derived classes
    String parseValueSubstitutingVariableReferences(const AtomicString&, WMLErrorCode defaultErrorCode = WMLErrorInvalidVariableReference) const;
    String parseValueForbiddingVariableReferences(const AtomicString&) const;
};

}

#endif
#endif
