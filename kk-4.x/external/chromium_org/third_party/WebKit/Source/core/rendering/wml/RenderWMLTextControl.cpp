/**
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 *           (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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
#include "core/rendering/wml/RenderWMLTextControl.h"

#include "core/wml/WMLInputElement.h"
#include "core/rendering/HitTestResult.h"
#include "core/rendering/RenderTheme.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "wtf/unicode/CharacterNames.h"

#include "base/debug/stack_trace.h"
#include <android/log.h>
#undef XLOG
#define XLOG(...) __android_log_print(ANDROID_LOG_DEBUG, "webview", __VA_ARGS__)

using namespace std;

namespace WebCore {

RenderWMLTextControl::RenderWMLTextControl(WMLInputElement* element)
    : RenderBlockFlow(element)
{
    ASSERT(element);
}

RenderWMLTextControl::~RenderWMLTextControl()
{
}

WMLInputElement* RenderWMLTextControl::textFormControlElement() const
{
    return toWMLInputElement(node());
}

Element* RenderWMLTextControl::innerTextElement() const
{
    return textFormControlElement()->innerTextElement();
}

void RenderWMLTextControl::addChild(RenderObject* newChild, RenderObject* beforeChild)
{
    // FIXME: This is a terrible hack to get the caret over the placeholder text since it'll
    // make us paint the placeholder first. (See https://trac.webkit.org/changeset/118733)
    Node* node = newChild->node();
    if (node && node->isElementNode() && toElement(node)->pseudo() == "-webkit-input-placeholder")
        RenderBlock::addChild(newChild, firstChild());
    else
        RenderBlock::addChild(newChild, beforeChild);
}

void RenderWMLTextControl::styleDidChange(StyleDifference diff, const RenderStyle* oldStyle)
{
    RenderBlock::styleDidChange(diff, oldStyle);

    Element* innerText = innerTextElement();
    XLOG("[RenderWMLTextControl::styleDidChange] [%p] node=[%p] innerText=[%p]"
                , this, node(), innerText);

    if (!innerText)
        return;
    RenderBlock* innerTextRenderer = toRenderBlock(innerText->renderer());
    if (innerTextRenderer) {
        // We may have set the width and the height in the old style in layout().
        // Reset them now to avoid getting a spurious layout hint.
        innerTextRenderer->style()->setHeight(Length());
        innerTextRenderer->style()->setWidth(Length());
        innerTextRenderer->setStyle(createInnerTextStyle(style()));
        innerText->setNeedsStyleRecalc();
    }
    //textFormControlElement()->updatePlaceholderVisibility(false);
}

static inline void updateUserModifyProperty(WMLInputElement* node, RenderStyle* style)
{
    style->setUserModify(node->isDisabledOrReadOnly() ? READ_ONLY : READ_WRITE_PLAINTEXT_ONLY);
}

void RenderWMLTextControl::adjustInnerTextStyle(RenderStyle* textBlockStyle) const
{
    // The inner block, if present, always has its direction set to LTR,
    // so we need to inherit the direction and unicode-bidi style from the element.
    textBlockStyle->setDirection(style()->direction());
    textBlockStyle->setUnicodeBidi(style()->unicodeBidi());

    updateUserModifyProperty(textFormControlElement(), textBlockStyle);
}

int RenderWMLTextControl::textBlockLogicalHeight() const
{
    return logicalHeight() - borderAndPaddingLogicalHeight();
}

int RenderWMLTextControl::textBlockLogicalWidth() const
{
    Element* innerText = innerTextElement();
    ASSERT(innerText);

    LayoutUnit unitWidth = logicalWidth() - borderAndPaddingLogicalWidth();
    if (innerText->renderer())
        unitWidth -= innerText->renderBox()->paddingStart() + innerText->renderBox()->paddingEnd();

    return unitWidth;
}

void RenderWMLTextControl::updateFromElement()
{
    Element* innerText = innerTextElement();
    if (innerText && innerText->renderer())
        updateUserModifyProperty(textFormControlElement(), innerText->renderer()->style());
}

int RenderWMLTextControl::scrollbarThickness() const
{
    // FIXME: We should get the size of the scrollbar from the RenderTheme instead.
    return ScrollbarTheme::theme()->scrollbarThickness();
}

void RenderWMLTextControl::computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    Element* innerText = innerTextElement();
    ASSERT(innerText);
    if (RenderBox* innerTextBox = innerText->renderBox()) {
        LayoutUnit nonContentHeight = innerTextBox->borderAndPaddingHeight() + innerTextBox->marginHeight();
        logicalHeight = computeControlLogicalHeight(innerTextBox->lineHeight(true, HorizontalLine, PositionOfInteriorLineBoxes), nonContentHeight) + borderAndPaddingHeight();

        // We are able to have a horizontal scrollbar if the overflow style is scroll, or if its auto and there's no word wrap.
        if ((isHorizontalWritingMode() && (style()->overflowX() == OSCROLL ||  (style()->overflowX() == OAUTO && innerText->renderer()->style()->overflowWrap() == NormalOverflowWrap)))
            || (!isHorizontalWritingMode() && (style()->overflowY() == OSCROLL ||  (style()->overflowY() == OAUTO && innerText->renderer()->style()->overflowWrap() == NormalOverflowWrap))))
            logicalHeight += scrollbarThickness();
    }

    RenderBox::computeLogicalHeight(logicalHeight, logicalTop, computedValues);
}

void RenderWMLTextControl::hitInnerTextElement(HitTestResult& result, const LayoutPoint& pointInContainer, const LayoutPoint& accumulatedOffset)
{
    Element* innerText = innerTextElement();
    if (!innerText->renderer())
        return;

    LayoutPoint adjustedLocation = accumulatedOffset + location();
    LayoutPoint localPoint = pointInContainer - toLayoutSize(adjustedLocation + innerText->renderBox()->location());
    if (hasOverflowClip())
        localPoint += scrolledContentOffset();
    result.setInnerNode(innerText);
    result.setInnerNonSharedNode(innerText);
    result.setLocalPoint(localPoint);
}

static const char* const fontFamiliesWithInvalidCharWidth[] = {
    "American Typewriter",
    "Arial Hebrew",
    "Chalkboard",
    "Cochin",
    "Corsiva Hebrew",
    "Courier",
    "Euphemia UCAS",
    "Geneva",
    "Gill Sans",
    "Hei",
    "Helvetica",
    "Hoefler Text",
    "InaiMathi",
    "Kai",
    "Lucida Grande",
    "Marker Felt",
    "Monaco",
    "Mshtakan",
    "New Peninim MT",
    "Osaka",
    "Raanana",
    "STHeiti",
    "Symbol",
    "Times",
    "Apple Braille",
    "Apple LiGothic",
    "Apple LiSung",
    "Apple Symbols",
    "AppleGothic",
    "AppleMyungjo",
    "#GungSeo",
    "#HeadLineA",
    "#PCMyungjo",
    "#PilGi",
};

// For font families where any of the fonts don't have a valid entry in the OS/2 table
// for avgCharWidth, fallback to the legacy webkit behavior of getting the avgCharWidth
// from the width of a '0'. This only seems to apply to a fixed number of Mac fonts,
// but, in order to get similar rendering across platforms, we do this check for
// all platforms.
bool RenderWMLTextControl::hasValidAvgCharWidth(AtomicString family)
{
    static HashSet<AtomicString>* fontFamiliesWithInvalidCharWidthMap = 0;

    if (family.isEmpty())
        return false;

    if (!fontFamiliesWithInvalidCharWidthMap) {
        fontFamiliesWithInvalidCharWidthMap = new HashSet<AtomicString>;

        for (size_t i = 0; i < WTF_ARRAY_LENGTH(fontFamiliesWithInvalidCharWidth); ++i)
            fontFamiliesWithInvalidCharWidthMap->add(AtomicString(fontFamiliesWithInvalidCharWidth[i]));
    }

    return !fontFamiliesWithInvalidCharWidthMap->contains(family);
}

float RenderWMLTextControl::getAvgCharWidth(AtomicString family)
{
    if (hasValidAvgCharWidth(family))
        return roundf(style()->font().primaryFont()->avgCharWidth());

    const UChar ch = '0';
    const String str = String(&ch, 1);
    const Font& font = style()->font();
    TextRun textRun = constructTextRun(this, font, str, style(), TextRun::AllowTrailingExpansion);
    textRun.disableRoundingHacks();
    return font.width(textRun);
}

float RenderWMLTextControl::scaleEmToUnits(int x) const
{
    // This matches the unitsPerEm value for MS Shell Dlg and Courier New from the "head" font table.
    float unitsPerEm = 2048.0f;
    return roundf(style()->font().size() * x / unitsPerEm);
}

void RenderWMLTextControl::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    // Use average character width. Matches IE.
    AtomicString family = style()->font().family().family();
    maxLogicalWidth = preferredContentLogicalWidth(const_cast<RenderWMLTextControl*>(this)->getAvgCharWidth(family));
    if (RenderBox* innerTextRenderBox = innerTextElement()->renderBox())
        maxLogicalWidth += innerTextRenderBox->paddingStart() + innerTextRenderBox->paddingEnd();
    if (!style()->logicalWidth().isPercent())
        minLogicalWidth = maxLogicalWidth;
}

void RenderWMLTextControl::computePreferredLogicalWidths()
{
    ASSERT(preferredLogicalWidthsDirty());

    m_minPreferredLogicalWidth = 0;
    m_maxPreferredLogicalWidth = 0;

    if (style()->logicalWidth().isFixed() && style()->logicalWidth().value() >= 0)
        m_minPreferredLogicalWidth = m_maxPreferredLogicalWidth = adjustContentBoxLogicalWidthForBoxSizing(style()->logicalWidth().value());
    else
        computeIntrinsicLogicalWidths(m_minPreferredLogicalWidth, m_maxPreferredLogicalWidth);

    if (style()->logicalMinWidth().isFixed() && style()->logicalMinWidth().value() > 0) {
        m_maxPreferredLogicalWidth = max(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(style()->logicalMinWidth().value()));
        m_minPreferredLogicalWidth = max(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(style()->logicalMinWidth().value()));
    }

    if (style()->logicalMaxWidth().isFixed()) {
        m_maxPreferredLogicalWidth = min(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(style()->logicalMaxWidth().value()));
        m_minPreferredLogicalWidth = min(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(style()->logicalMaxWidth().value()));
    }

    LayoutUnit toAdd = borderAndPaddingLogicalWidth();

    m_minPreferredLogicalWidth += toAdd;
    m_maxPreferredLogicalWidth += toAdd;

    clearPreferredLogicalWidthsDirty();
}

void RenderWMLTextControl::addFocusRingRects(Vector<IntRect>& rects, const LayoutPoint& additionalOffset, const RenderLayerModelObject*)
{
    if (!size().isEmpty())
        rects.append(pixelSnappedIntRect(additionalOffset, size()));
}

RenderObject* RenderWMLTextControl::layoutSpecialExcludedChild(bool relayoutChildren, SubtreeLayoutScope& layoutScope)
{
    HTMLElement* placeholder = toHTMLTextFormControlElement(node())->placeholderElement();
    RenderObject* placeholderRenderer = placeholder ? placeholder->renderer() : 0;
    if (!placeholderRenderer)
        return 0;
    if (relayoutChildren)
        layoutScope.setChildNeedsLayout(placeholderRenderer);
    return placeholderRenderer;
}

} // namespace WebCore

#endif
