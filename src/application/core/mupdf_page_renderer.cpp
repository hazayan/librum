#include "mupdf_page_renderer.hpp"
#include <math.h>
#include <cmath>
#include "fz_utils.hpp"
#include "mupdf/classes.h"
#include "mupdf/classes2.h"
#include "mupdf/fitz/geometry.h"

namespace application::core
{

using utils::FzPointPair;

application::core::MupdfPageRenderer::MupdfPageRenderer(
    std::unique_ptr<mupdf::FzPage> page,
    std::unique_ptr<mupdf::FzStextPage> sTextPage) :
    m_page(std::move(page)),
    m_textPage(std::move(sTextPage)),
    m_textSelector(nullptr)
{
    auto boundPage = m_page->fz_bound_page();
    setupDisplayList(boundPage);

    // If the text page was not provided, the MupdfPageRenderer is in
    // render-only mode, meaning that the only safe funciton to call is
    // "renderPage()". This is useful for e.g. rendering covers.
    if(!m_textPage)
        m_renderOnly = true;

    if(!m_renderOnly)
    {
        setupSymbolBounds();
        setupLinks();
        m_textSelector = utils::TextSelector(m_textPage.get());
    }
}

void MupdfPageRenderer::setupDisplayList(const mupdf::FzRect& boundPage)
{
    m_displayList = mupdf::FzDisplayList(boundPage);

    auto listDevice = m_displayList.fz_new_list_device();
    mupdf::FzCookie defaultCookie;
    m_page->fz_run_page(listDevice, mupdf::FzMatrix(), defaultCookie);
    listDevice.fz_close_device();
}

void MupdfPageRenderer::setupSymbolBounds()
{
    auto curr = m_textPage->begin();
    auto end = m_textPage->end();

    m_symbolBounds.resize(100);
    while(curr != end)
    {
        auto symbol = curr;
        fz_rect rect = symbol->m_internal->bbox;
        m_symbolBounds.emplace_back(rect);
        ++curr;
    }
}

void MupdfPageRenderer::setupLinks()
{
    auto end = m_page->fz_load_links().end();
    auto curr = m_page->fz_load_links().begin();
    while(curr != end)
    {
        m_links.push_back(*curr);
        ++curr;
    }
}

mupdf::FzPixmap MupdfPageRenderer::renderPage(float zoom)
{
    // Create matrix with zoom
    mupdf::FzMatrix matrix;
    matrix.a = zoom;
    matrix.d = zoom;

    auto pixmap = getEmptyPixmap(matrix);
    auto drawDevice = mupdf::fz_new_draw_device(mupdf::FzMatrix(), pixmap);

    // Determine the page offset the first time we render the page
    if(!m_renderOnly && (m_pageXOffset == 0 && m_pageYOffset == 0))
    {
        int restoredXOffset = round(pixmap.x() / zoom);
        int restoredYOffset = round(pixmap.y() / zoom);
        if(restoredXOffset != 0 && restoredYOffset != 0)
        {
            setPageOffsets(restoredXOffset, restoredYOffset);

            std::vector<fz_rect> newSymbolBounds;
            for(auto& rect : m_symbolBounds)
            {
                auto newRect = fz_make_rect(
                    rect.x0 - m_pageXOffset, rect.y0 - m_pageYOffset,
                    rect.x1 - m_pageXOffset, rect.y1 - m_pageYOffset);

                newSymbolBounds.emplace_back(newRect);
            }
            m_symbolBounds = newSymbolBounds;

            for(auto& link : m_links)
            {
                auto newLinkRect =
                    utils::moveRect(link.rect(), m_pageXOffset, m_pageYOffset);
                link.fz_set_link_rect(newLinkRect);
            }
        }
    }

    mupdf::FzCookie cookie;
    mupdf::FzRect rect = mupdf::FzRect::Fixed_INFINITE;
    m_displayList.fz_run_display_list(drawDevice, matrix, rect, cookie);
    drawDevice.fz_close_device();

    if(m_invertColor)
        pixmap.fz_invert_pixmap();

    return pixmap;
}

mupdf::FzPixmap MupdfPageRenderer::getEmptyPixmap(
    const mupdf::FzMatrix& matrix) const
{
    auto bbox = m_page->fz_bound_page_box(FZ_CROP_BOX);
    auto scaledBbox = bbox.fz_transform_rect(matrix);

    mupdf::FzPixmap pixmap(mupdf::FzColorspace::Fixed_RGB, scaledBbox,
                           mupdf::FzSeparations(), 0);
    pixmap.fz_clear_pixmap();

    return pixmap;
}

void MupdfPageRenderer::setPageOffsets(int xOffset, int yOffset)
{
    m_pageXOffset = xOffset;
    m_pageYOffset = yOffset;

    m_textSelector.setPageXOffset(xOffset);
    m_textSelector.setPageYOffset(yOffset);
}

void imageCleanupHandler(void* data)
{
    unsigned char* samples = static_cast<unsigned char*>(data);
    delete[] samples;
}

int MupdfPageRenderer::getWidth() const
{
    auto bbox = m_page->fz_bound_page_box(FZ_CROP_BOX);

    return (bbox.x1 - bbox.x0);
}

int MupdfPageRenderer::getHeight() const
{
    auto bbox = m_page->fz_bound_page_box(FZ_CROP_BOX);

    return (bbox.y1 - bbox.y0);
}

int MupdfPageRenderer::getPageXOffset() const
{
    return m_pageXOffset;
}

int MupdfPageRenderer::getPageYOffset() const
{
    return m_pageYOffset;
}

QList<mupdf::FzQuad>& MupdfPageRenderer::getBufferedSelectionRects()
{
    return m_bufferedSelectionRects;
}

void MupdfPageRenderer::setInvertColor(bool newInvertColor)
{
    if(m_invertColor == newInvertColor)
        return;

    m_invertColor = newInvertColor;
}

void MupdfPageRenderer::generateSelectionRects(mupdf::FzPoint start,
                                               mupdf::FzPoint end)
{
    m_textSelector.generateSelectionRects(m_bufferedSelectionRects, start, end);
}

FzPointPair MupdfPageRenderer::getPositionsForWordSelection(
    mupdf::FzPoint begin, mupdf::FzPoint end)
{
    return m_textSelector.getPositionsForWordSelection(begin, end);
}

FzPointPair MupdfPageRenderer::getPositionsForLineSelection(
    mupdf::FzPoint point)
{
    return m_textSelector.getPositionsForLineSelection(point);
}

std::string MupdfPageRenderer::getTextFromSelection(mupdf::FzPoint start,
                                                    mupdf::FzPoint end)
{
    return m_textSelector.getTextFromSelection(start, end);
}

bool MupdfPageRenderer::pointIsAboveText(mupdf::FzPoint point)
{
    for(auto& rect : m_symbolBounds)
    {
        if(point.fz_is_point_inside_rect(rect))
            return true;
    }

    return false;
}

bool MupdfPageRenderer::pointIsAboveLink(mupdf::FzPoint point)
{
    for(auto& link : m_links)
    {
        if(point.fz_is_point_inside_rect(link.rect()))
            return true;
    }

    return false;
}

mupdf::FzLink MupdfPageRenderer::getLinkAtPoint(mupdf::FzPoint point)
{
    for(auto& link : m_links)
    {
        if(point.fz_is_point_inside_rect(link.rect()))
            return link;
    }

    return mupdf::FzLink();
}

}  // namespace application::core
