/*
 * Copyright (C) 2006, 2007, 2008, 2011 Apple Inc. All rights reserved.
 *               2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(WML)
#include "core/rendering/wml/RenderWMLListBox.h"

#include <math.h>
#include "WMLNames.h"
#include "core/accessibility/AXObjectCache.h"
#include "core/css/CSSFontSelector.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/Document.h"
#include "core/dom/NodeRenderStyle.h"
#include "core/editing/FrameSelection.h"
#include "core/html/HTMLOptGroupElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/page/EventHandler.h"
#include "core/page/FocusController.h"
#include "core/frame/Frame.h"
#include "core/frame/FrameView.h"
#include "core/page/Page.h"
#include "core/page/SpatialNavigation.h"
#include "core/rendering/HitTestResult.h"
#include "core/rendering/LayoutRectRecorder.h"
#include "core/rendering/PaintInfo.h"
#include "core/rendering/RenderScrollbar.h"
#include "core/rendering/RenderText.h"
#include "core/rendering/RenderTheme.h"
#include "core/rendering/RenderView.h"
#include "platform/fonts/FontCache.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/scroll/Scrollbar.h"

#include "core/wml/WMLOptGroupElement.h"
#include "core/wml/WMLOptionElement.h"
#include "core/wml/WMLSelectElement.h"

using namespace std;

namespace WebCore {

using namespace WMLNames;

const int rowSpacing = 1;

const int optionsSpacingHorizontal = 2;

// The minSize constant was originally defined to render scrollbars correctly.
// This might vary for different platforms.
const int minSize = 4;

// Default size when the multiple attribute is present but size attribute is absent.
const int defaultSize = 4;

// FIXME: This hardcoded baselineAdjustment is what we used to do for the old
// widget, but I'm not sure this is right for the new control.
const int baselineAdjustment = 7;

RenderWMLListBox::RenderWMLListBox(Element* element)
    : RenderBlockFlow(element)
    , m_optionsChanged(true)
    , m_scrollToRevealSelectionAfterLayout(true)
    , m_inAutoscroll(false)
    , m_optionsWidth(0)
    , m_indexOffset(0)
{
    ASSERT(element);
    ASSERT(element->isWMLElement());
    ASSERT(element->hasTagName(HTMLNames::selectTag));

    if (FrameView* frameView = frame()->view())
        frameView->addScrollableArea(this);
}

RenderWMLListBox::~RenderWMLListBox()
{
    setHasVerticalScrollbar(false);

    if (FrameView* frameView = frame()->view())
        frameView->removeScrollableArea(this);
}

inline WMLSelectElement* RenderWMLListBox::selectElement() const
{
    return toWMLSelectElement(node());
}

void RenderWMLListBox::updateFromElement()
{
    FontCachePurgePreventer fontCachePurgePreventer;

    if (m_optionsChanged) {
        /// M: @{
        const Vector<Element*>& listItems = selectElement()->listItems();
        /// @}
        int size = numItems();

        float width = 0;
        for (int i = 0; i < size; ++i) {
            /// M: @{
            Element* element = listItems[i];
            /// @}
            String text;
            Font itemFont = style()->font();
            if (element->hasTagName(optionTag)) {
                text = toWMLOptionElement(element)->textIndentedToRespectGroupLabel();
            } else if (isWMLOptGroupElement(element)) {
                text = toWMLOptGroupElement(element)->groupLabelText();
                FontDescription d = itemFont.fontDescription();
                d.setWeight(d.bolderWeight());
                itemFont = Font(d, itemFont.letterSpacing(), itemFont.wordSpacing());
                itemFont.update(document().styleEngine()->fontSelector());
            }

            if (!text.isEmpty()) {
                applyTextTransform(style(), text, ' ');
                // FIXME: Why is this always LTR? Can't text direction affect the width?
                TextRun textRun = constructTextRun(this, itemFont, text, style(), TextRun::AllowTrailingExpansion);
                textRun.disableRoundingHacks();
                float textWidth = itemFont.width(textRun);
                width = max(width, textWidth);
            }
        }
        m_optionsWidth = static_cast<int>(ceilf(width));
        m_optionsChanged = false;

        setHasVerticalScrollbar(true);

        setNeedsLayoutAndPrefWidthsRecalc();
    }
}

void RenderWMLListBox::selectionChanged()
{
    repaint();
    if (!m_inAutoscroll) {
        if (m_optionsChanged || needsLayout())
            m_scrollToRevealSelectionAfterLayout = true;
        else
            scrollToRevealSelection();
    }

    if (AXObjectCache* cache = document().existingAXObjectCache())
        cache->selectedChildrenChanged(this);
}

void RenderWMLListBox::layout()
{
    LayoutRectRecorder recorder(*this);
    RenderBlockFlow::layout();

    if (m_vBar) {
        bool enabled = numVisibleItems() < numItems();
        m_vBar->setEnabled(enabled);
        m_vBar->setProportion(numVisibleItems(), numItems());
        if (!enabled) {
            scrollToOffsetWithoutAnimation(VerticalScrollbar, 0);
            m_indexOffset = 0;
        }
    }

    if (m_scrollToRevealSelectionAfterLayout) {
        LayoutStateDisabler layoutStateDisabler(view());
        scrollToRevealSelection();
    }
}

void RenderWMLListBox::scrollToRevealSelection()
{
    WMLSelectElement* select = selectElement();

    m_scrollToRevealSelectionAfterLayout = false;

    int firstIndex = select->activeSelectionStartListIndex();
    if (firstIndex >= 0 && !listIndexIsVisible(select->activeSelectionEndListIndex()))
        scrollToRevealElementAtListIndex(firstIndex);
}

void RenderWMLListBox::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    maxLogicalWidth = m_optionsWidth + 2 * optionsSpacingHorizontal;
    if (m_vBar)
        maxLogicalWidth += verticalScrollbarWidth();
    if (!style()->width().isPercent())
        minLogicalWidth = maxLogicalWidth;
}

void RenderWMLListBox::computePreferredLogicalWidths()
{
    ASSERT(!m_optionsChanged);

    m_minPreferredLogicalWidth = 0;
    m_maxPreferredLogicalWidth = 0;

    if (style()->width().isFixed() && style()->width().value() > 0)
        m_minPreferredLogicalWidth = m_maxPreferredLogicalWidth = adjustContentBoxLogicalWidthForBoxSizing(style()->width().value());
    else
        computeIntrinsicLogicalWidths(m_minPreferredLogicalWidth, m_maxPreferredLogicalWidth);

    if (style()->minWidth().isFixed() && style()->minWidth().value() > 0) {
        m_maxPreferredLogicalWidth = max(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(style()->minWidth().value()));
        m_minPreferredLogicalWidth = max(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(style()->minWidth().value()));
    }

    if (style()->maxWidth().isFixed()) {
        m_maxPreferredLogicalWidth = min(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(style()->maxWidth().value()));
        m_minPreferredLogicalWidth = min(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(style()->maxWidth().value()));
    }

    LayoutUnit toAdd = borderAndPaddingWidth();
    m_minPreferredLogicalWidth += toAdd;
    m_maxPreferredLogicalWidth += toAdd;

    clearPreferredLogicalWidthsDirty();
}

int RenderWMLListBox::size() const
{
    int specifiedSize = selectElement()->size();
    if (specifiedSize > 1)
        return max(minSize, specifiedSize);

    return defaultSize;
}

int RenderWMLListBox::numVisibleItems() const
{
    // Only count fully visible rows. But don't return 0 even if only part of a row shows.
    return max<int>(1, (contentHeight() + rowSpacing) / itemHeight());
}

int RenderWMLListBox::numItems() const
{
    return selectElement()->listItems().size();
}

LayoutUnit RenderWMLListBox::listHeight() const
{
    return itemHeight() * numItems() - rowSpacing;
}

void RenderWMLListBox::computeLogicalHeight(LayoutUnit, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    LayoutUnit height = itemHeight() * size() - rowSpacing + borderAndPaddingHeight();
    RenderBox::computeLogicalHeight(height, logicalTop, computedValues);
}

int RenderWMLListBox::baselinePosition(FontBaseline baselineType, bool firstLine, LineDirectionMode lineDirection, LinePositionMode linePositionMode) const
{
    return RenderBox::baselinePosition(baselineType, firstLine, lineDirection, linePositionMode) - baselineAdjustment;
}

LayoutRect RenderWMLListBox::itemBoundingBoxRect(const LayoutPoint& additionalOffset, int index)
{
    // For RTL, items start after the left-side vertical scrollbar.
    int scrollbarOffset = style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft() ? verticalScrollbarWidth() : 0;
    return LayoutRect(additionalOffset.x() + borderLeft() + paddingLeft() + scrollbarOffset,
        additionalOffset.y() + borderTop() + paddingTop() + itemHeight() * (index - m_indexOffset),
        contentWidth(), itemHeight());
}

void RenderWMLListBox::paintObject(PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (style()->visibility() != VISIBLE)
        return;

    int listItemsSize = numItems();

    if (paintInfo.phase == PaintPhaseForeground) {
        int index = m_indexOffset;
        while (index < listItemsSize && index <= m_indexOffset + numVisibleItems()) {
            paintItemForeground(paintInfo, paintOffset, index);
            index++;
        }
    }

    // Paint the children.
    RenderBlock::paintObject(paintInfo, paintOffset);

    switch (paintInfo.phase) {
    // Depending on whether we have overlay scrollbars they
    // get rendered in the foreground or background phases
    case PaintPhaseForeground:
        if (m_vBar && m_vBar->isOverlayScrollbar())
            paintScrollbar(paintInfo, paintOffset);
        break;
    case PaintPhaseBlockBackground:
        if (m_vBar && !m_vBar->isOverlayScrollbar())
            paintScrollbar(paintInfo, paintOffset);
        break;
    case PaintPhaseChildBlockBackground:
    case PaintPhaseChildBlockBackgrounds: {
        int index = m_indexOffset;
        while (index < listItemsSize && index <= m_indexOffset + numVisibleItems()) {
            paintItemBackground(paintInfo, paintOffset, index);
            index++;
        }
        break;
    }
    default:
        break;
    }
}

void RenderWMLListBox::addFocusRingRects(Vector<IntRect>& rects, const LayoutPoint& additionalOffset, const RenderLayerModelObject* paintContainer)
{
    if (!isSpatialNavigationEnabled(frame()))
        return RenderBlock::addFocusRingRects(rects, additionalOffset, paintContainer);

    WMLSelectElement* select = selectElement();

    // Focus the last selected item.
    int selectedItem = select->activeSelectionEndListIndex();
    if (selectedItem >= 0) {
        rects.append(pixelSnappedIntRect(itemBoundingBoxRect(additionalOffset, selectedItem)));
        return;
    }

    // No selected items, find the first non-disabled item.
    int size = numItems();
    /// M: @{
    const Vector<Element*>& listItems = select->listItems();
    /// @}
    for (int i = 0; i < size; ++i) {
        /// M: @{
        Element* element = listItems[i];
        /// @}
        if (element->hasTagName(optionTag) && !element->isDisabledFormControl()) {
            rects.append(pixelSnappedIntRect(itemBoundingBoxRect(additionalOffset, i)));
            return;
        }
    }
}

int RenderWMLListBox::scrollbarLeft() const
{
    int scrollbarLeft = 0;
    if (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        scrollbarLeft = borderLeft();
    else
        scrollbarLeft = width() - borderRight() - verticalScrollbarWidth();
    return scrollbarLeft;
}


void RenderWMLListBox::paintScrollbar(PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (m_vBar) {
        IntRect scrollRect = pixelSnappedIntRect(paintOffset.x() + scrollbarLeft(),
            paintOffset.y() + borderTop(),
            verticalScrollbarWidth(),
            height() - (borderTop() + borderBottom()));
        m_vBar->setFrameRect(scrollRect);
        m_vBar->paint(paintInfo.context, paintInfo.rect);
    }
}

static LayoutSize itemOffsetForAlignment(TextRun textRun, RenderStyle* itemStyle, Font itemFont, LayoutRect itemBoudingBox)
{
    ETextAlign actualAlignment = itemStyle->textAlign();
    // FIXME: Firefox doesn't respect JUSTIFY. Should we?
    // FIXME: Handle TAEND here
    if (actualAlignment == TASTART || actualAlignment == JUSTIFY)
      actualAlignment = itemStyle->isLeftToRightDirection() ? LEFT : RIGHT;

    LayoutSize offset = LayoutSize(0, itemFont.fontMetrics().ascent());
    if (actualAlignment == RIGHT || actualAlignment == WEBKIT_RIGHT) {
        float textWidth = itemFont.width(textRun);
        offset.setWidth(itemBoudingBox.width() - textWidth - optionsSpacingHorizontal);
    } else if (actualAlignment == CENTER || actualAlignment == WEBKIT_CENTER) {
        float textWidth = itemFont.width(textRun);
        offset.setWidth((itemBoudingBox.width() - textWidth) / 2);
    } else
        offset.setWidth(optionsSpacingHorizontal);
    return offset;
}

void RenderWMLListBox::paintItemForeground(PaintInfo& paintInfo, const LayoutPoint& paintOffset, int listIndex)
{
    FontCachePurgePreventer fontCachePurgePreventer;

    WMLSelectElement* select = selectElement();

    /// M: @{
    const Vector<Element*>& listItems = select->listItems();
    Element* element = listItems[listIndex];
    /// @}

    RenderStyle* itemStyle = element->renderStyle();
    if (!itemStyle)
        itemStyle = style();

    if (itemStyle->visibility() == HIDDEN)
        return;

    String itemText;
    bool isOptionElement = element->hasTagName(optionTag);
    if (isOptionElement)
        itemText = toWMLOptionElement(element)->textIndentedToRespectGroupLabel();
    else if (isWMLOptGroupElement(element))
        itemText = toWMLOptGroupElement(element)->groupLabelText();
    applyTextTransform(style(), itemText, ' ');

    Color textColor = element->renderStyle() ? resolveColor(element->renderStyle(), CSSPropertyColor) : resolveColor(CSSPropertyColor);
    if (isOptionElement && toHTMLOptionElement(element)->selected()) {
        if (frame()->selection().isFocusedAndActive() && document().focusedElement() == node())
            textColor = RenderTheme::theme().activeListBoxSelectionForegroundColor();
        // Honor the foreground color for disabled items
        else if (!element->isDisabledFormControl() && !select->isDisabledFormControl())
            textColor = RenderTheme::theme().inactiveListBoxSelectionForegroundColor();
    }

    paintInfo.context->setFillColor(textColor);

    TextRun textRun(itemText, 0, 0, TextRun::AllowTrailingExpansion, itemStyle->direction(), isOverride(itemStyle->unicodeBidi()), true, TextRun::NoRounding);
    Font itemFont = style()->font();
    LayoutRect r = itemBoundingBoxRect(paintOffset, listIndex);
    r.move(itemOffsetForAlignment(textRun, itemStyle, itemFont, r));

    if (isWMLOptGroupElement(element)) {
        FontDescription d = itemFont.fontDescription();
        d.setWeight(d.bolderWeight());
        itemFont = Font(d, itemFont.letterSpacing(), itemFont.wordSpacing());
        itemFont.update(document().styleEngine()->fontSelector());
    }

    // Draw the item text
    TextRunPaintInfo textRunPaintInfo(textRun);
    textRunPaintInfo.bounds = r;
    paintInfo.context->drawBidiText(itemFont, textRunPaintInfo, roundedIntPoint(r.location()));
}

void RenderWMLListBox::paintItemBackground(PaintInfo& paintInfo, const LayoutPoint& paintOffset, int listIndex)
{
    /// M: @{
    const Vector<Element*>& listItems = selectElement()->listItems();
    Element* element = listItems[listIndex];
    /// @}

    Color backColor;
    if (element->hasTagName(optionTag) && toHTMLOptionElement(element)->selected()) {
        if (frame()->selection().isFocusedAndActive() && document().focusedElement() == node())
            backColor = RenderTheme::theme().activeListBoxSelectionBackgroundColor();
        else
            backColor = RenderTheme::theme().inactiveListBoxSelectionBackgroundColor();
    } else {
        backColor = element->renderStyle() ? resolveColor(element->renderStyle(), CSSPropertyBackgroundColor) : resolveColor(CSSPropertyBackgroundColor);
    }

    // Draw the background for this list box item
    if (!element->renderStyle() || element->renderStyle()->visibility() != HIDDEN) {
        LayoutRect itemRect = itemBoundingBoxRect(paintOffset, listIndex);
        itemRect.intersect(controlClipRect(paintOffset));
        paintInfo.context->fillRect(pixelSnappedIntRect(itemRect), backColor);
    }
}

bool RenderWMLListBox::isPointInOverflowControl(HitTestResult& result, const LayoutPoint& locationInContainer, const LayoutPoint& accumulatedOffset)
{
    if (!m_vBar || !m_vBar->shouldParticipateInHitTesting())
        return false;

    LayoutRect vertRect(accumulatedOffset.x() + scrollbarLeft(),
                        accumulatedOffset.y() + borderTop(),
                        verticalScrollbarWidth(),
                        height() - borderTop() - borderBottom());

    if (vertRect.contains(locationInContainer)) {
        result.setScrollbar(m_vBar.get());
        return true;
    }
    return false;
}

int RenderWMLListBox::listIndexAtOffset(const LayoutSize& offset)
{
    if (!numItems())
        return -1;

    if (offset.height() < borderTop() + paddingTop() || offset.height() > height() - paddingBottom() - borderBottom())
        return -1;

    int scrollbarWidth = verticalScrollbarWidth();
    int rightScrollbarOffset = style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft() ? scrollbarWidth : 0;
    int leftScrollbarOffset = style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft() ? 0 : scrollbarWidth;
    if (offset.width() < borderLeft() + paddingLeft() + rightScrollbarOffset
        || offset.width() > width() - borderRight() - paddingRight() - leftScrollbarOffset)
        return -1;

    int newOffset = (offset.height() - borderTop() - paddingTop()) / itemHeight() + m_indexOffset;
    return newOffset < numItems() ? newOffset : -1;
}

void RenderWMLListBox::panScroll(const IntPoint& panStartMousePosition)
{
    const int maxSpeed = 20;
    const int iconRadius = 7;
    const int speedReducer = 4;

    // FIXME: This doesn't work correctly with transforms.
    FloatPoint absOffset = localToAbsolute();

    IntPoint lastKnownMousePosition = frame()->eventHandler().lastKnownMousePosition();
    // We need to check if the last known mouse position is out of the window. When the mouse is out of the window, the position is incoherent
    static IntPoint previousMousePosition;
    if (lastKnownMousePosition.y() < 0)
        lastKnownMousePosition = previousMousePosition;
    else
        previousMousePosition = lastKnownMousePosition;

    int yDelta = lastKnownMousePosition.y() - panStartMousePosition.y();

    // If the point is too far from the center we limit the speed
    yDelta = max<int>(min<int>(yDelta, maxSpeed), -maxSpeed);

    if (abs(yDelta) < iconRadius) // at the center we let the space for the icon
        return;

    if (yDelta > 0)
        //offsetY = view()->viewHeight();
        absOffset.move(0, listHeight());
    else if (yDelta < 0)
        yDelta--;

    // Let's attenuate the speed
    yDelta /= speedReducer;

    IntPoint scrollPoint(0, 0);
    scrollPoint.setY(absOffset.y() + yDelta);
    int newOffset = scrollToward(scrollPoint);
    if (newOffset < 0)
        return;

    m_inAutoscroll = true;
    WMLSelectElement* select = selectElement();
    select->updateListBoxSelection(!select->multiple());
    m_inAutoscroll = false;
}

int RenderWMLListBox::scrollToward(const IntPoint& destination)
{
    // FIXME: This doesn't work correctly with transforms.
    FloatPoint absPos = localToAbsolute();
    IntSize positionOffset = roundedIntSize(destination - absPos);

    int rows = numVisibleItems();
    int offset = m_indexOffset;

    if (positionOffset.height() < borderTop() + paddingTop() && scrollToRevealElementAtListIndex(offset - 1))
        return offset - 1;

    if (positionOffset.height() > height() - paddingBottom() - borderBottom() && scrollToRevealElementAtListIndex(offset + rows))
        return offset + rows - 1;

    return listIndexAtOffset(positionOffset);
}

void RenderWMLListBox::autoscroll(const IntPoint&)
{
    IntPoint pos = frame()->view()->windowToContents(frame()->eventHandler().lastKnownMousePosition());

    int endIndex = scrollToward(pos);
    if (selectElement()->isDisabledFormControl())
        return;

    if (endIndex >= 0) {
        WMLSelectElement* select = selectElement();
        m_inAutoscroll = true;

        if (!select->multiple())
            select->setActiveSelectionAnchorIndex(endIndex);

        select->setActiveSelectionEndIndex(endIndex);
        select->updateListBoxSelection(!select->multiple());
        m_inAutoscroll = false;
    }
}

void RenderWMLListBox::stopAutoscroll()
{
    if (selectElement()->isDisabledFormControl())
        return;

    selectElement()->listBoxOnChange();
}

bool RenderWMLListBox::scrollToRevealElementAtListIndex(int index)
{
    if (index < 0 || index >= numItems() || listIndexIsVisible(index))
        return false;

    int newOffset;
    if (index < m_indexOffset)
        newOffset = index;
    else
        newOffset = index - numVisibleItems() + 1;

    scrollToOffsetWithoutAnimation(VerticalScrollbar, newOffset);

    return true;
}

bool RenderWMLListBox::listIndexIsVisible(int index)
{
    return index >= m_indexOffset && index < m_indexOffset + numVisibleItems();
}

bool RenderWMLListBox::scroll(ScrollDirection direction, ScrollGranularity granularity, float multiplier)
{
    return ScrollableArea::scroll(direction, granularity, multiplier);
}

void RenderWMLListBox::valueChanged(unsigned listIndex)
{
    WMLSelectElement* element = selectElement();
    element->setSelectedIndex(element->listToOptionIndex(listIndex));
    element->dispatchFormControlChangeEvent();
}

int RenderWMLListBox::scrollSize(ScrollbarOrientation orientation) const
{
    return orientation == VerticalScrollbar ? (numItems() - numVisibleItems()) : 0;
}

IntPoint RenderWMLListBox::scrollPosition() const
{
    return IntPoint(0, m_indexOffset);
}

void RenderWMLListBox::setScrollOffset(const IntPoint& offset)
{
    scrollTo(offset.y());
}

void RenderWMLListBox::scrollTo(int newOffset)
{
    if (newOffset == m_indexOffset)
        return;

    m_indexOffset = newOffset;
    repaint();
    node()->document().enqueueScrollEventForNode(node());
}

LayoutUnit RenderWMLListBox::itemHeight() const
{
    return style()->fontMetrics().height() + rowSpacing;
}

int RenderWMLListBox::verticalScrollbarWidth() const
{
    return m_vBar && !m_vBar->isOverlayScrollbar() ? m_vBar->width() : 0;
}

// FIXME: We ignore padding in the vertical direction as far as these values are concerned, since that's
// how the control currently paints.
int RenderWMLListBox::scrollWidth() const
{
    // There is no horizontal scrolling allowed.
    return pixelSnappedClientWidth();
}

int RenderWMLListBox::scrollHeight() const
{
    return max(pixelSnappedClientHeight(), roundToInt(listHeight()));
}

int RenderWMLListBox::scrollLeft() const
{
    return 0;
}

void RenderWMLListBox::setScrollLeft(int)
{
}

int RenderWMLListBox::scrollTop() const
{
    return m_indexOffset * itemHeight();
}

void RenderWMLListBox::setScrollTop(int newTop)
{
    // Determine an index and scroll to it.
    int index = newTop / itemHeight();
    if (index < 0 || index >= numItems() || index == m_indexOffset)
        return;

    scrollToOffsetWithoutAnimation(VerticalScrollbar, index);
}

bool RenderWMLListBox::nodeAtPoint(const HitTestRequest& request, HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction)
{
    if (!RenderBlock::nodeAtPoint(request, result, locationInContainer, accumulatedOffset, hitTestAction))
        return false;
    /// M: @{
    const Vector<Element*>& listItems = selectElement()->listItems();
    /// @}
    int size = numItems();
    LayoutPoint adjustedLocation = accumulatedOffset + location();

    for (int i = 0; i < size; ++i) {
        if (itemBoundingBoxRect(adjustedLocation, i).contains(locationInContainer.point())) {
            if (Element* node = listItems[i]) {
                result.setInnerNode(node);
                if (!result.innerNonSharedNode())
                    result.setInnerNonSharedNode(node);
                result.setLocalPoint(locationInContainer.point() - toLayoutSize(adjustedLocation));
                break;
            }
        }
    }

    return true;
}

LayoutRect RenderWMLListBox::controlClipRect(const LayoutPoint& additionalOffset) const
{
    LayoutRect clipRect = contentBoxRect();
    if (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        clipRect.moveBy(additionalOffset + LayoutPoint(verticalScrollbarWidth(), 0));
    else
        clipRect.moveBy(additionalOffset);
    return clipRect;
}

bool RenderWMLListBox::isActive() const
{
    Page* page = frame()->page();
    return page && page->focusController().isActive();
}

void RenderWMLListBox::invalidateScrollbarRect(Scrollbar* scrollbar, const IntRect& rect)
{
    IntRect scrollRect = rect;
    if (style()->shouldPlaceBlockDirectionScrollbarOnLogicalLeft())
        scrollRect.move(borderLeft(), borderTop());
    else
        scrollRect.move(width() - borderRight() - scrollbar->width(), borderTop());
    repaintRectangle(scrollRect);
}

IntRect RenderWMLListBox::convertFromScrollbarToContainingView(const Scrollbar* scrollbar, const IntRect& scrollbarRect) const
{
    RenderView* view = this->view();
    if (!view)
        return scrollbarRect;

    IntRect rect = scrollbarRect;

    int scrollbarTop = borderTop();
    rect.move(scrollbarLeft(), scrollbarTop);

    return view->frameView()->convertFromRenderer(this, rect);
}

IntRect RenderWMLListBox::convertFromContainingViewToScrollbar(const Scrollbar* scrollbar, const IntRect& parentRect) const
{
    RenderView* view = this->view();
    if (!view)
        return parentRect;

    IntRect rect = view->frameView()->convertToRenderer(this, parentRect);

    int scrollbarTop = borderTop();
    rect.move(-scrollbarLeft(), -scrollbarTop);
    return rect;
}

IntPoint RenderWMLListBox::convertFromScrollbarToContainingView(const Scrollbar* scrollbar, const IntPoint& scrollbarPoint) const
{
    RenderView* view = this->view();
    if (!view)
        return scrollbarPoint;

    IntPoint point = scrollbarPoint;

    int scrollbarTop = borderTop();
    point.move(scrollbarLeft(), scrollbarTop);

    return view->frameView()->convertFromRenderer(this, point);
}

IntPoint RenderWMLListBox::convertFromContainingViewToScrollbar(const Scrollbar* scrollbar, const IntPoint& parentPoint) const
{
    RenderView* view = this->view();
    if (!view)
        return parentPoint;

    IntPoint point = view->frameView()->convertToRenderer(this, parentPoint);

    int scrollbarTop = borderTop();
    point.move(-scrollbarLeft(), -scrollbarTop);
    return point;
}

IntSize RenderWMLListBox::contentsSize() const
{
    return IntSize(scrollWidth(), scrollHeight());
}

int RenderWMLListBox::visibleHeight() const
{
    return height();
}

int RenderWMLListBox::visibleWidth() const
{
    return width();
}

IntPoint RenderWMLListBox::lastKnownMousePosition() const
{
    RenderView* view = this->view();
    if (!view)
        return IntPoint();
    return view->frameView()->lastKnownMousePosition();
}

bool RenderWMLListBox::shouldSuspendScrollAnimations() const
{
    RenderView* view = this->view();
    if (!view)
        return true;
    return view->frameView()->shouldSuspendScrollAnimations();
}

bool RenderWMLListBox::scrollbarsCanBeActive() const
{
    RenderView* view = this->view();
    if (!view)
        return false;
    return view->frameView()->scrollbarsCanBeActive();
}

IntPoint RenderWMLListBox::minimumScrollPosition() const
{
    return IntPoint();
}

IntPoint RenderWMLListBox::maximumScrollPosition() const
{
    return IntPoint(0, numItems() - numVisibleItems());
}

bool RenderWMLListBox::userInputScrollable(ScrollbarOrientation orientation) const
{
    return orientation == VerticalScrollbar;
}

bool RenderWMLListBox::shouldPlaceVerticalScrollbarOnLeft() const
{
    return false;
}

int RenderWMLListBox::lineStep(ScrollbarOrientation) const
{
    return 1;
}

int RenderWMLListBox::pageStep(ScrollbarOrientation orientation) const
{
    return max(1, numVisibleItems() - 1);
}

float RenderWMLListBox::pixelStep(ScrollbarOrientation) const
{
    return 1.0f / itemHeight();
}

ScrollableArea* RenderWMLListBox::enclosingScrollableArea() const
{
    // FIXME: Return a RenderLayer that's scrollable.
    return 0;
}

IntRect RenderWMLListBox::scrollableAreaBoundingBox() const
{
    return absoluteBoundingBoxRect();
}

PassRefPtr<Scrollbar> RenderWMLListBox::createScrollbar()
{
    RefPtr<Scrollbar> widget;
    bool hasCustomScrollbarStyle = style()->hasPseudoStyle(SCROLLBAR);
    if (hasCustomScrollbarStyle)
        widget = RenderScrollbar::createCustomScrollbar(this, VerticalScrollbar, this->node());
    else {
        widget = Scrollbar::create(this, VerticalScrollbar, RenderTheme::theme().scrollbarControlSizeForPart(ListboxPart));
        didAddScrollbar(widget.get(), VerticalScrollbar);
    }
    document().view()->addChild(widget.get());
    return widget.release();
}

void RenderWMLListBox::destroyScrollbar()
{
    if (!m_vBar)
        return;

    if (!m_vBar->isCustomScrollbar())
        ScrollableArea::willRemoveScrollbar(m_vBar.get(), VerticalScrollbar);
    m_vBar->removeFromParent();
    m_vBar->disconnectFromScrollableArea();
    m_vBar = 0;
}

void RenderWMLListBox::setHasVerticalScrollbar(bool hasScrollbar)
{
    if (hasScrollbar == (m_vBar != 0))
        return;

    if (hasScrollbar)
        m_vBar = createScrollbar();
    else
        destroyScrollbar();

    if (m_vBar)
        m_vBar->styleChanged();

    // Force an update since we know the scrollbars have changed things.
    if (document().hasAnnotatedRegions())
        document().setAnnotatedRegionsDirty(true);
}

} // namespace WebCore

#endif
