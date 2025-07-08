#include "page_renderer.hpp"
#include <QDebug>
#include "fz_utils.hpp"
#include "mupdf/classes.h"

using namespace application::core;

namespace adapters::core
{

PageRenderer::PageRenderer(IDocumentAccess* document, int pageNumber,
                           double dpr) :
    m_mupdfPageRenderer(document->loadPage(pageNumber),
                        document->loadTextPage(pageNumber)),
    m_dpr(dpr)
{
}

int PageRenderer::getWidth() const
{
    return m_mupdfPageRenderer.getWidth() * m_matrix.a / m_dpr;
}

int PageRenderer::getHeight() const
{
    return m_mupdfPageRenderer.getHeight() * m_matrix.d / m_dpr;
}

int PageRenderer::getXOffset() const
{
    return m_pageXOffset;
}

int PageRenderer::getYOffset() const
{
    return m_pageYOffset;
}

void PageRenderer::setZoom(float zoom)
{
    m_matrix.a = zoom * m_dpr;
    m_matrix.d = zoom * m_dpr;

    m_pageImageOutdated = true;
    m_selectionRectsOutdated = true;
}

float PageRenderer::getZoom() const
{
    return m_matrix.a;
}

void PageRenderer::setInvertColor(bool newInvertColor)
{
    m_mupdfPageRenderer.setInvertColor(newInvertColor);
    m_pageImageOutdated = true;
}

QImage PageRenderer::renderPage()
{
    if(!m_pageImageOutdated)
        return m_pageImage;

    auto zoom = m_matrix.a;
    m_pageImage = utils::qImageFromPixmap(m_mupdfPageRenderer.renderPage(zoom));

    auto xOffset = m_mupdfPageRenderer.getPageXOffset();
    auto yOffset = m_mupdfPageRenderer.getPageYOffset();
    if(xOffset != m_pageXOffset || yOffset != m_pageYOffset)
    {
        m_pageXOffset = xOffset;
        m_pageYOffset = yOffset;
        emit pageOffsetsChanged(xOffset, yOffset);
    }

    m_pageImageOutdated = false;
    return m_pageImage;
}

bool PageRenderer::pointIsAboveText(const QPointF& point)
{
    auto fzPoint = utils::qPointToFzPoint(point, m_dpr);
    utils::restoreFzPoint(fzPoint, m_matrix);

    return m_mupdfPageRenderer.pointIsAboveText(fzPoint);
}

bool PageRenderer::pointIsAboveLink(const QPointF& point)
{
    auto fzPoint = utils::qPointToFzPoint(point, m_dpr);
    utils::restoreFzPoint(fzPoint, m_matrix);

    return m_mupdfPageRenderer.pointIsAboveLink(fzPoint);
}

const char* PageRenderer::getLinkUriAtPoint(const QPointF& point)
{
    auto fzPoint = utils::qPointToFzPoint(point, m_dpr);
    utils::restoreFzPoint(fzPoint, m_matrix);

    return m_mupdfPageRenderer.getLinkAtPoint(fzPoint).uri();
}

const QList<QRectF>& PageRenderer::getBufferedSelectionRects()
{
    if(!m_selectionRectsOutdated)
        return m_selectionRects;

    m_selectionRects.clear();
    auto& rects = m_mupdfPageRenderer.getBufferedSelectionRects();
    rects.reserve(rects.size());
    for(auto& rect : rects)
    {
        rect = rect.fz_transform_quad(m_matrix);
        m_selectionRects.append(utils::fzQuadToQRectF(rect));
    }

    m_selectionRectsOutdated = false;
    return m_selectionRects;
}

void PageRenderer::generateSelectionRects(QPointF start, QPointF end)
{
    auto fzStart = utils::qPointToFzPoint(start, m_dpr);
    auto fzEnd = utils::qPointToFzPoint(end, m_dpr);
    utils::restoreFzPoint(fzStart, m_matrix);
    utils::restoreFzPoint(fzEnd, m_matrix);

    m_mupdfPageRenderer.getBufferedSelectionRects().clear();
    m_mupdfPageRenderer.generateSelectionRects(fzStart, fzEnd);

    m_selectionRectsOutdated = true;
}

void PageRenderer::clearBufferedSelectionRects()
{
    m_mupdfPageRenderer.getBufferedSelectionRects().clear();
    m_selectionRectsOutdated = true;
}

QPair<QPointF, QPointF> PageRenderer::getPositionsForWordSelection(
    QPointF start, QPointF end)
{
    auto fzStart = utils::qPointToFzPoint(start, m_dpr);
    auto fzEnd = utils::qPointToFzPoint(end, m_dpr);
    utils::restoreFzPoint(fzStart, m_matrix);
    utils::restoreFzPoint(fzEnd, m_matrix);

    auto res = m_mupdfPageRenderer.getPositionsForWordSelection(fzStart, fzEnd);
    res.first = res.first.fz_transform_point(m_matrix);
    res.second = res.second.fz_transform_point(m_matrix);

    return QPointFPair(utils::fzPointToQPoint(res.first, m_dpr),
                       utils::fzPointToQPoint(res.second, m_dpr));
}

QPair<QPointF, QPointF> PageRenderer::getPositionsForLineSelection(
    QPointF point)
{
    auto fzPoint = utils::qPointToFzPoint(point, m_dpr);
    utils::restoreFzPoint(fzPoint, m_matrix);

    auto res = m_mupdfPageRenderer.getPositionsForLineSelection(fzPoint);
    res.first = res.first.fz_transform_point(m_matrix);
    res.second = res.second.fz_transform_point(m_matrix);

    return QPointFPair(utils::fzPointToQPoint(res.first, m_dpr),
                       utils::fzPointToQPoint(res.second, m_dpr));
}

QString PageRenderer::getTextFromSelection(const QPointF& start,
                                           const QPointF& end)
{
    auto fzStart = utils::qPointToFzPoint(start, m_dpr);
    auto fzEnd = utils::qPointToFzPoint(end, m_dpr);
    utils::restoreFzPoint(fzStart, m_matrix);
    utils::restoreFzPoint(fzEnd, m_matrix);

    auto res = m_mupdfPageRenderer.getTextFromSelection(fzStart, fzEnd);
    return QString::fromStdString(res);
}

}  // namespace adapters::core
