#include "page.hpp"
#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRectF>
#include <QSGSimpleTextureNode>
#include <QtWidgets/QApplication>
#include <limits>
#include "fz_utils.hpp"
#include "highlight.hpp"
#include "page_renderer.hpp"

using adapters::IDocumentAccess;
using adapters::IOpenedBookController;
using adapters::core::PageRenderer;
using domain::entities::Highlight;
using namespace application::core;

namespace cpp_elements
{

cpp_elements::Page::Page(QObject* parent,
                         IOpenedBookController* openedBookController,
                         IDocumentAccess* documentAccess, int pageNumber,
                         float zoom, qreal devicePixelRatio) :
    m_openedBookController(openedBookController),
    m_pageNumber(pageNumber)
{
    setParent(parent);

    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);

    m_tripleClickTimer.setInterval(400);
    m_tripleClickTimer.setSingleShot(true);

    m_pageRenderer = std::make_unique<PageRenderer>(documentAccess, pageNumber,
                                                    devicePixelRatio);
    m_pageRenderer->setZoom(zoom);
}

int Page::getImplicitWidth() const
{
    return m_pageRenderer->getWidth();
}

int Page::getImplicitHeight() const
{
    return m_pageRenderer->getHeight();
}

int Page::getPageNumber() const
{
    return m_pageNumber;
}

void Page::setZoom(float newZoom)
{
    auto oldZoom = m_pageRenderer->getZoom();
    m_pageRenderer->setZoom(newZoom);

    // Update selection positions to match new zoom
    if(!m_selectionStart.isNull() && !m_selectionEnd.isNull())
    {
        m_selectionStart =
            utils::scalePointToCurrentZoom(m_selectionStart, oldZoom, newZoom);
        m_selectionEnd =
            utils::scalePointToCurrentZoom(m_selectionEnd, oldZoom, newZoom);

        m_pageRenderer->generateSelectionRects(m_selectionStart,
                                               m_selectionEnd);
    }

    emit implicitWidthChanged();
    emit implicitHeightChanged();
}

void Page::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if(newGeometry.size().isEmpty())
        return;

    if(newGeometry.width() != oldGeometry.width() ||
       newGeometry.height() != newGeometry.height())
    {
        update();
    }

    QQuickItem::geometryChange(newGeometry, oldGeometry);
}

QSGNode* Page::updatePaintNode(QSGNode* node, UpdatePaintNodeData* nodeData)
{
    Q_UNUSED(nodeData);
    QSGSimpleTextureNode* n = static_cast<QSGSimpleTextureNode*>(node);
    if(!n)
    {
        n = new QSGSimpleTextureNode();
        n->setOwnsTexture(true);
    }

    auto image = m_pageRenderer->renderPage();
    QPainter painter(&image);

    paintSelectionOnPage(painter);
    paintHighlightsOnPage(painter);

    n->setTexture(window()->createTextureFromImage(image));
    n->setRect(boundingRect());
    return n;
}

void Page::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
        return;

    if(m_startedMousePressOnHighlight)
        return;

    int mouseX = event->position().x();
    int mouseY = event->position().y();
    QPoint mousePoint(mouseX, mouseY);

    m_selectionStart = mousePoint;
    m_selectionEnd = mousePoint;
    selectSingleWord();

    m_tripleClickTimer.start();
    m_doubleClickHold = true;
}

void Page::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
        return;

    int mouseX = event->position().x();
    int mouseY = event->position().y();
    QPoint point(mouseX, mouseY);

    forceActiveFocus();

    if(m_pageRenderer->pointIsAboveLink(point))
        m_startedMousePressOnLink = true;

    auto highlight = m_openedBookController->getHighlightAtPoint(
        point, m_pageNumber, m_pageRenderer->getZoom());
    if(highlight != nullptr)
    {
        handleClickingOnHighlight(highlight);
        return;
    }
    m_startedMousePressOnHighlight = false;

    // Select line when left mouse button is pressed 3 times
    if(m_tripleClickTimer.isActive())
    {
        selectLine();
        return;
    }

    m_selectionStart = point;
}

void Page::handleClickingOnHighlight(const Highlight* highlight)
{
    // Scale the highlight rects (initially zoom of 1) to the current zoom and
    // get the center x and bottom y position of the highlight.
    auto rects = highlight->getRects();
    QList<QRectF> qRects;
    qRects.reserve(rects.size());
    for(auto& rect : rects)
    {
        auto qRectF = rect.getQRect();
        utils::scaleQRectFToZoom(
            qRectF, m_pageRenderer->getZoom() / window()->devicePixelRatio());
        qRects.append(qRectF);
    }
    auto positions = getCenterXAndBottomYFromRects(qRects);

    auto uuidAsString = highlight->getUuid().toString(QUuid::WithoutBraces);
    emit highlightSelected(positions.first, positions.second, uuidAsString);
    m_startedMousePressOnHighlight = true;
}

void Page::mouseReleaseEvent(QMouseEvent* event)
{
    int mouseX = event->position().x();
    int mouseY = event->position().y();
    QPoint mousePoint(mouseX, mouseY);

    if(m_startedMousePressOnLink &&
       m_pageRenderer->pointIsAboveLink(mousePoint))
    {
        auto uri = m_pageRenderer->getLinkUriAtPoint(mousePoint);
        emit followLink(uri);
    }
    m_startedMousePressOnLink = false;

    // This gets triggered when the user simply clicks on the page, without
    // dragging the mouse, so on a normal click. In this case we want to
    // reset the selection.
    if(m_selectionStart == QPointF(mouseX, mouseY) &&
       !m_tripleClickTimer.isActive())
    {
        removeSelection();

        // Restart it since some actions are checking if it is active to e.g.
        // prevent removing the line select on mouse release
        m_tripleClickTimer.start();
    }
    else if(!m_startedMousePressOnHighlight)
    {
        auto rects = m_pageRenderer->getBufferedSelectionRects();
        QList<QRectF> restoredRects;
        restoredRects.reserve(rects.size());
        for(auto rect : rects)
        {
            utils::restoreQRect(rect, m_pageRenderer->getZoom());
            utils::scaleQRectFToZoom(
                rect, m_pageRenderer->getZoom() / window()->devicePixelRatio());
            restoredRects.push_back(rect);
        }

        auto positions = getCenterXAndBottomYFromRects(restoredRects);
        emit textSelectionFinished(positions.first, positions.second);
    }

    m_doubleClickHold = false;
}

QPair<float, float> Page::getCenterXAndBottomYFromRects(
    const QList<QRectF>& rects)
{
    float mostLeftX = std::numeric_limits<float>::max();
    float mostRightX = 0;
    float bottomY = 0;
    for(auto& rect : rects)
    {
        if(rect.x() < mostLeftX)
            mostLeftX = rect.x();

        if(rect.x() + rect.width() > mostRightX)
            mostRightX = rect.x() + rect.width();

        if(rect.bottom() > bottomY)
            bottomY = rect.bottom();
    }

    auto centerX = (mostLeftX + mostRightX) / 2;
    return { centerX, bottomY };
}

void Page::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() == Qt::RightButton)
        return;

    emit mouseHoverMoved(event->pos().x(), event->pos().y());

    if(m_startedMousePressOnHighlight)
        return;

    int mouseX = event->position().x();
    int mouseY = event->position().y();

    // 'hoverMoveEvent' is not triggered when the left mouse button is pressed,
    // thus the cursor will not change correctly. Make sure to handle it here.
    setCorrectCursor(mouseX, mouseY);

    m_selectionEnd = QPointF(mouseX, mouseY);
    if(m_doubleClickHold)
        selectMultipleWords();
    else
        createSelection();
}

void Page::hoverMoveEvent(QHoverEvent* event)
{
    double mouseX = event->position().x();
    double mouseY = event->position().y();
    setCorrectCursor(mouseX, mouseY);

    emit mouseHoverMoved(mouseX, mouseY);
}

void Page::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier)
    {
        copySelectedText();
    }
}

void Page::hoverLeaveEvent(QHoverEvent* event)
{
    emit resetCursor();
}

void Page::paintSelectionOnPage(QPainter& painter)
{
    auto& selectionRects = m_pageRenderer->getBufferedSelectionRects();
    for(auto rect : selectionRects)
    {
        QColor selectionColor(134, 171, 175, 125);
        painter.setCompositionMode(QPainter::CompositionMode_Multiply);
        painter.fillRect(rect, selectionColor);
    }
}

void Page::paintHighlightsOnPage(QPainter& painter)
{
    for(auto& highlight : m_openedBookController->getHighlights())
    {
        if(highlight.getPageNumber() != m_pageNumber)
            continue;

        for(auto rect : highlight.getRects())
        {
            // We store the highlights zoom independent, so we need to scale
            // them to the current zoom here.
            auto qRect = rect.getQRect();
            utils::scaleQRectFToZoom(qRect, m_pageRenderer->getZoom());
            rect.setQRect(qRect);

            painter.setCompositionMode(QPainter::CompositionMode_Multiply);
            painter.fillRect(rect.getQRect(), highlight.getColor());
        }
    }
}

void Page::removeConflictingHighlights(Highlight& highlight)
{
    bool existingHighlightRemoved = false;

    auto& highlights = m_openedBookController->getHighlights();
    for(int i = 0; i < highlights.size(); ++i)
    {
        auto& existingHighlight = highlights[i];
        if(existingHighlight.getPageNumber() != highlight.getPageNumber())
            continue;

        for(int u = 0; u < highlight.getRects().size(); ++u)
        {
            auto rect = highlight.getRects()[u].getQRect();

            for(int k = 0; k < existingHighlight.getRects().size(); ++k)
            {
                auto existingRect = existingHighlight.getRects()[k].getQRect();

                // New rect intersects with old rect
                if(rect.intersects(existingRect))
                {
                    // Make sure that the rects are on the same line. Depending
                    // on the line height, lines above eachother will overlap,
                    // but its only an intersect when they are on the same line.
                    bool onSameLine = rectsAreOnSameLine(existingRect, rect);
                    if(onSameLine)
                    {
                        auto uuid = highlights[i].getUuid();
                        m_openedBookController->removeHighlight(uuid);
                        --i;
                        existingHighlightRemoved = true;
                        break;
                    }
                }
            }

            if(existingHighlightRemoved)
            {
                existingHighlightRemoved = false;
                break;
            }
        }
    }
}

bool Page::mouseAboveSelection(const QPointF mouse)
{
    auto& selectionRects = m_pageRenderer->getBufferedSelectionRects();
    for(auto& rect : selectionRects)
    {
        if(rect.contains(mouse))
            return true;
    }

    return false;
}

QString Page::createHighlightFromCurrentSelection(const QString& hex, int alpha)
{
    auto bufferedSelectionRects = m_pageRenderer->getBufferedSelectionRects();

    // Make sure to restore the rects to their original size, since we want to
    // store them, so they need to be zoom independent.
    for(auto& rect : bufferedSelectionRects)
    {
        utils::restoreQRect(rect, m_pageRenderer->getZoom());
    }

    removeSelection();

    auto color = QColor(hex);
    color.setAlpha(alpha);
    auto rects = bufferedSelectionRects;
    Highlight highlight(m_pageNumber, color);
    highlight.setRects(rects);

    removeConflictingHighlights(highlight);
    m_openedBookController->addHighlight(highlight);

    update();
    return highlight.getUuid().toString(QUuid::WithoutBraces);
}

void Page::removeHighlight(const QString& uuid)
{
    m_openedBookController->removeHighlight(QUuid(uuid));

    update();
}

void Page::changeHighlightColor(const QString& uuid, const QString& color,
                                int alpha)
{
    QColor newColor(color);
    newColor.setAlpha(alpha);

    m_openedBookController->changeHighlightColor(QUuid(uuid), newColor);

    update();
}

void Page::copyHighlightedText(const QString& uuid)
{
    auto text = getHighlightedText(uuid);

    auto clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

QString Page::getSelectedText()
{
    return m_pageRenderer->getTextFromSelection(m_selectionStart,
                                                m_selectionEnd);
}

QString Page::getHighlightedText(const QString& uuid)
{
    const Highlight* highlight = nullptr;
    for(auto& h : m_openedBookController->getHighlights())
    {
        if(h.getUuid() == QUuid(uuid))
        {
            highlight = &h;
            break;
        }
    }

    QPointF start(highlight->getRects().first().getQRect().left(),
                  highlight->getRects().first().getQRect().center().y());

    QPointF end(highlight->getRects().last().getQRect().right(),
                highlight->getRects().last().getQRect().center().y());

    start = utils::scalePointToCurrentZoom(start, 1, m_pageRenderer->getZoom());
    end = utils::scalePointToCurrentZoom(end, 1, m_pageRenderer->getZoom());

    return m_pageRenderer->getTextFromSelection(start, end);
}

void Page::selectTextBetweenPoints(QPointF left, QPointF right)
{
    m_pageRenderer->renderPage();

    auto xOffset = m_pageRenderer->getXOffset();
    auto yOffset = m_pageRenderer->getYOffset();
    left = QPoint(left.x() - xOffset, left.y() - yOffset);
    right = QPoint(right.x() - xOffset, right.y() - yOffset);

    // The points received from this signal are relative to a zoom
    // of 1, but all of the methods in this class handle points as
    // if they have the currentZoom applied, so we need to scale it.
    // We need to divide by the dpr since the lower level methods
    // expect the caller not to know anything about the dpr, but the
    // current zoom we get from the page controller is zoom * dpr.
    // Thus we need to reset it to the zoom without the dpr.
    auto zoom = m_pageRenderer->getZoom();
    auto dpr = window()->devicePixelRatio();
    left = utils::scalePointToCurrentZoom(left, 1, zoom / dpr);
    right = utils::scalePointToCurrentZoom(right, 1, zoom / dpr);

    m_selectionStart = left;
    m_selectionEnd = right;

    createSelection();
}

void Page::setPointingCursor()
{
    if(QApplication::overrideCursor() == nullptr ||
       *QApplication::overrideCursor() != Qt::PointingHandCursor)
    {
        emit resetCursor();
        emit updateCursor(Qt::PointingHandCursor);
    }
}

void Page::createSelection()
{
    m_pageRenderer->generateSelectionRects(m_selectionStart, m_selectionEnd);
    update();
}

void Page::removeSelection()
{
    m_pageRenderer->clearBufferedSelectionRects();
    update();

    m_selectionStart = QPointF(0, 0);
    m_selectionEnd = QPointF(0, 0);
}

void Page::selectSingleWord()
{
    auto points = m_pageRenderer->getPositionsForWordSelection(m_selectionStart,
                                                               m_selectionEnd);

    m_selectionStart = points.first;
    m_selectionEnd = points.second;

    createSelection();
}

void Page::selectMultipleWords()
{
    auto positions = m_pageRenderer->getPositionsForWordSelection(
        m_selectionStart, m_selectionEnd);

    m_selectionStart = positions.first;
    m_selectionEnd = positions.second;

    createSelection();
}

void Page::selectLine()
{
    auto positions =
        m_pageRenderer->getPositionsForLineSelection(m_selectionStart);

    m_selectionStart = positions.first;
    m_selectionEnd = positions.second;

    createSelection();
}

void Page::copySelectedText()
{
    QString text = getSelectedText();
    if(!m_includeNewLinesInCopiedText)
    {
        text.replace("\n", "");
        text.replace("\r", "");
    }

    auto clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void Page::setCorrectCursor(int x, int y)
{
    if(m_pageRenderer->pointIsAboveLink(QPoint(x, y)) ||
       m_openedBookController->getHighlightAtPoint(QPointF(x, y), m_pageNumber,
                                                   m_pageRenderer->getZoom()))
    {
        setPointingCursor();
    }
    else if(m_pageRenderer->pointIsAboveText(QPoint(x, y)))
    {
        if(QApplication::overrideCursor() == nullptr ||
           *QApplication::overrideCursor() != Qt::IBeamCursor)
        {
            emit resetCursor();
            emit updateCursor(Qt::IBeamCursor);
        }
    }
    else
    {
        emit resetCursor();
    }
}

bool Page::rectsAreOnSameLine(const QRectF& rect1, const QRectF& rect2)
{
    auto shorterRect = rect1.height() <= rect2.height() ? rect1 : rect2;
    auto intersectH = rect1.intersected(rect2).height();

    float offsetTolerance = 0.75;
    bool onSameLine = intersectH >= shorterRect.height() * offsetTolerance;

    return onSameLine;
}

void Page::setColorInverted(bool newColorInverted)
{
    // This method gets called on initialisation of the page item, but we don't
    // want to redraw it then, so we skip it if it's called for the first time.
    m_pageRenderer->setInvertColor(newColorInverted);
    if(!m_firstTimeColorInverted)
        update();

    m_firstTimeColorInverted = false;
}

void Page::setIncludeNewLinesInCopiedText(bool newIncludeNewLinesInCopiedText)
{
    m_includeNewLinesInCopiedText = newIncludeNewLinesInCopiedText;
}

float Page::getYOffset() const
{
    return m_pageRenderer->getYOffset();
}

}  // namespace cpp_elements
