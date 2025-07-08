#pragma once
#include <QPoint>
#include <QQuickItem>
#include <QString>
#include <QTimer>
#include "i_document_access.hpp"
#include "i_opened_book_controller.hpp"
#include "i_page_renderer.hpp"
#include "presentation_export.hpp"

namespace cpp_elements
{

/**
 * This class is responsible for rendering a single page of the book.
 * It derives from QQuickItem, which is the base class for all visual item
 * types in Qml. It is a visual item that can be drawn on the screen and
 * interacted with.
 */
class PRESENTATION_EXPORT Page : public QQuickItem
{
    Q_OBJECT

public:
    Page(QObject* parent, adapters::IOpenedBookController* openedBookController,
         adapters::IDocumentAccess* documentAccess, int pageNumber, float zoom,
         qreal devicePixelRatio);

    int getImplicitWidth() const;
    int getImplicitHeight() const;

    void setZoom(float newZoom);

    float getYOffset() const;

    bool disableHoverEvents() const;
    void setDisableHoverEvents(bool newDisableHoverEvents);

    int getPageNumber() const;
    void setColorInverted(bool newColorInverted);
    void setIncludeNewLinesInCopiedText(bool newIncludeNewLinesInCopiedText);
    void syncChanges();

    Q_INVOKABLE void copySelectedText();
    Q_INVOKABLE void copyHighlightedText(const QString& uuid);
    Q_INVOKABLE QString getSelectedText();
    Q_INVOKABLE QString getHighlightedText(const QString& uuid);
    Q_INVOKABLE void selectTextBetweenPoints(QPointF left, QPointF right);

    Q_INVOKABLE void removeSelection();
    Q_INVOKABLE void setPointingCursor();
    Q_INVOKABLE QString createHighlightFromCurrentSelection(const QString& hex,
                                                            int alpha);
    Q_INVOKABLE void removeHighlight(const QString& uuid);
    Q_INVOKABLE void changeHighlightColor(const QString& uuid,
                                          const QString& color, int alpha);

signals:
    void mouseHoverMoved(double x, double y);
    void followLink(const char* uri);
    void textSelectionFinished(float centerX, float topY);
    void highlightSelected(float centerX, float topY, const QString& uuid);
    void updateCursor(Qt::CursorShape cursor);
    void resetCursor();

protected:
    void geometryChange(const QRectF& newGeometry,
                        const QRectF& oldGeometry) override;
    QSGNode* updatePaintNode(QSGNode* node, UpdatePaintNodeData* _) override;

    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void hoverMoveEvent(QHoverEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void hoverLeaveEvent(QHoverEvent* event) override;

private:
    void selectSingleWord();
    void selectMultipleWords();
    void selectLine();
    void createSelection();
    void paintSelectionOnPage(QPainter& painter);

    void paintHighlightsOnPage(QPainter& painter);
    void handleClickingOnHighlight(
        const domain::entities::Highlight* highlight);
    void removeConflictingHighlights(domain::entities::Highlight& highlight);
    bool mouseAboveSelection(const QPointF mouse);

    void setCorrectCursor(int x, int y);

    bool rectsAreOnSameLine(const QRectF& rect1, const QRectF& rect2);
    QPair<float, float> getCenterXAndBottomYFromRects(
        const QList<QRectF>& rects);

    adapters::IOpenedBookController* m_openedBookController = nullptr;
    std::unique_ptr<adapters::IPageRenderer> m_pageRenderer;
    QPointF m_selectionStart;
    QPointF m_selectionEnd;
    QTimer m_tripleClickTimer;
    int m_pageNumber = 0;
    bool m_firstTimeColorInverted = true;
    bool m_startedMousePressOnLink = false;
    bool m_startedMousePressOnHighlight = false;
    bool m_doubleClickHold = false;
    bool m_includeNewLinesInCopiedText = false;
};

}  // namespace cpp_elements
