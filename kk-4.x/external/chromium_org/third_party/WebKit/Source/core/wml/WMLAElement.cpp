/**
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2003, 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
 *           (C) 2006 Graham Dennis (graham.dennis@gmail.com)
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
#include "WMLAElement.h"

#include "core/dom/Attribute.h"
#include "core/events/Event.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/frame/Frame.h"
#include "core/html/HTMLAnchorElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/page/EventHandler.h"
#include "core/page/Chrome.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "core/rendering/RenderBox.h"
#include "platform/network/DNS.h"
#include "platform/network/ResourceRequest.h"
#include "HTMLNames.h"
#include "WMLNames.h"
/// M: Substitute WML variable
#include "WMLVariables.h"

namespace WebCore {

using namespace WMLNames;

WMLAElement::WMLAElement(const QualifiedName& tagName, Document& document)
    : WMLElement(tagName, document)
{
}

PassRefPtr<WMLAElement> WMLAElement::create(Document& document)
{
    return adoptRef(new WMLAElement(aTag, document));
}

void WMLAElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::hrefAttr) {
        bool wasLink = isLink();
        setIsLink(!value.isNull());
        if (wasLink != isLink())
            setNeedsStyleRecalc();
        if (isLink() && document().isDNSPrefetchEnabled()) {
            if (protocolIs(value, "http") || protocolIs(value, "https") || value.startsWith("//"))
                prefetchDNS(document().completeURL(value).host());
        }
    } else if (name == HTMLNames::nameAttr
               || name == HTMLNames::titleAttr
               || name == HTMLNames::relAttr) {
        // Do nothing.
    } else
        WMLElement::parseAttribute(name, value);
}

bool WMLAElement::supportsFocus() const
{
    return isLink() || WMLElement::supportsFocus();
}

bool WMLAElement::isMouseFocusable() const
{
    return false;
}

bool WMLAElement::isKeyboardFocusable(KeyboardEvent* event) const
{
    if (!isFocusable())
        return false;
    
    if (!document().frame())
        return false;

    Page* page = document().page();
    if (!page)
        return false;

    if (!page->chrome().client().tabsToLinks())
        return false;

    if (!renderer() || !renderer()->isBoxModelObject())
        return false;

    return hasNonEmptyBoundingBox();
}

void WMLAElement::defaultEventHandler(Event* event)
{
    if (isLink()) {
        if (focused() && isEnterKeyKeydownEvent(event)) {
            event->setDefaultHandled();
            dispatchSimulatedClick(event);
            return;
        }

        if (isLinkClick(event)) {
            /// M: Substitute WML variable @{
            //String href = substituteVariableReferences(getAttribute(HTMLNames::hrefAttr), document(), WMLVariableEscapingEscape);
            //handleLinkClick(event, document(), stripLeadingAndTrailingHTMLSpaces(href), target(), event);
            /// }@
            handleClick(event);
            return;
        }
    }

    WMLElement::defaultEventHandler(event);
}


void WMLAElement::handleClick(Event* event)
{
    event->setDefaultHandled();

    Frame* frame = document().frame();
    if (!frame)
        return;

    String url = substituteVariableReferences(getAttribute(HTMLNames::hrefAttr), document(), WMLVariableEscapingEscape);
    KURL completedURL = document().completeURL(url);

    ResourceRequest request(completedURL);
    FrameLoadRequest frameRequest(&document(), request, target());
    frameRequest.setTriggeringEvent(event);
    frame->loader().load(frameRequest);
}

void WMLAElement::accessKeyAction(bool sendMouseEvents)
{
    dispatchSimulatedClick(0, sendMouseEvents ? SendMouseUpDownEvents : SendNoEvents);
}

bool WMLAElement::isURLAttribute(Attribute *attr) const
{
    return attr->name() == HTMLNames::hrefAttr;
}

String WMLAElement::target() const
{
    return getAttribute(HTMLNames::targetAttr);
}

/// M: Substitute WML variable
KURL WMLAElement::href() const
{
    String href = substituteVariableReferences(getAttribute(HTMLNames::hrefAttr), document(), WMLVariableEscapingEscape);
    return document().completeURL(href);
}

}

#endif
