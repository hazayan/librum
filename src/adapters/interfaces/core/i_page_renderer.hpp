#pragma once
#include <QObject>
#include "adapters_export.hpp"

namespace adapters
{

class ADAPTERS_EXPORT IPageRenderer : public QObject
{
    Q_OBJECT

public:
    virtual ~IPageRenderer() noexcept = default;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    virtual int getXOffset() const = 0;
    virtual int getYOffset() const = 0;

    virtual void setZoom(float zoom) = 0;
    virtual float getZoom() const = 0;

    virtual void setInvertColor(bool newInvertColor) = 0;

    virtual QImage renderPage() = 0;

    virtual bool pointIsAboveText(const QPointF& point) = 0;
    virtual bool pointIsAboveLink(const QPointF& point) = 0;
    virtual const char* getLinkUriAtPoint(const QPointF& point) = 0;

    virtual const QList<QRectF>& getBufferedSelectionRects() = 0;
    virtual void generateSelectionRects(QPointF start, QPointF end) = 0;
    virtual void clearBufferedSelectionRects() = 0;

    virtual QPair<QPointF, QPointF> getPositionsForWordSelection(
        QPointF start, QPointF end) = 0;
    virtual QPair<QPointF, QPointF> getPositionsForLineSelection(
        QPointF point) = 0;
    virtual QString getTextFromSelection(const QPointF& start,
                                         const QPointF& end) = 0;
};

}  // namespace adapters
