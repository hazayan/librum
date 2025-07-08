#pragma once
#include <QObject>
#include "adapters_export.hpp"
#include "i_document_access.hpp"
#include "i_page_renderer.hpp"
#include "mupdf_page_renderer.hpp"

namespace adapters::core
{

class ADAPTERS_EXPORT PageRenderer : public IPageRenderer
{
    Q_OBJECT

public:
    PageRenderer(IDocumentAccess* document, int pageNumber, double dpr);

    int getWidth() const;
    int getHeight() const;

    int getXOffset() const;
    int getYOffset() const;

    void setZoom(float zoom);
    float getZoom() const;

    void setInvertColor(bool newInvertColor);

    QImage renderPage();

    bool pointIsAboveText(const QPointF& point);
    bool pointIsAboveLink(const QPointF& point);
    const char* getLinkUriAtPoint(const QPointF& point);

    const QList<QRectF>& getBufferedSelectionRects();
    void generateSelectionRects(QPointF start, QPointF end);
    void clearBufferedSelectionRects();

    QPair<QPointF, QPointF> getPositionsForWordSelection(QPointF start,
                                                         QPointF end);
    QPair<QPointF, QPointF> getPositionsForLineSelection(QPointF point);
    QString getTextFromSelection(const QPointF& start, const QPointF& end);

signals:
    void pageOffsetsChanged(int xOffset, int yOffset);

private:
    application::core::MupdfPageRenderer m_mupdfPageRenderer;
    mupdf::FzMatrix m_matrix;

    int m_pageXOffset = 0;
    int m_pageYOffset = 0;

    // The dpr is the "device pixel ratio" which is a ratio (e.g. 1.25) between
    // the device pixels and the logical pixels. For example, if the screen is
    // scaled by 125% via software, the dpr will be 1.25.
    // We need to make sure that the page is rendered with the correct dpr so
    // that the page is not blurry.
    double m_dpr = 1.0;

    // Image caching
    bool m_pageImageOutdated = true;
    QImage m_pageImage;

    // Selection rects outdated
    bool m_selectionRectsOutdated = true;
    QList<QRectF> m_selectionRects;

    using QPointFPair = QPair<QPointF, QPointF>;
};

}  // namespace adapters::core
