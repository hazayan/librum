#include "document_view.hpp"
#include <QRandomGenerator>
#include <QSGImageNode>
#include <QSGSimpleRectNode>
#include "test_rect.hpp"

namespace cpp_elements
{

DocumentView::DocumentView()
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFlag(QQuickItem::ItemAcceptsInputMethod, true);

    // We need to find a good value for this after rendering the first
    // page I assume
    setImplicitWidth(500);
}

void DocumentView::setBookController(
    adapters::IBookController* newBookController)
{
    m_bookController = newBookController;

    updatePages();
}

QSGNode* DocumentView::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    return oldNode;
}

void DocumentView::wheelEvent(QWheelEvent* event)
{
    int deltaY = event->angleDelta().y();
    if(m_controlPressed)
    {
        handleZoom(deltaY);
    }
    else
    {
        handleScroll(deltaY);
    }

    event->accept();
    update();
}

void DocumentView::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Control)
        m_controlPressed = true;

    event->accept();
}

void DocumentView::keyReleaseEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Control)
        m_controlPressed = false;

    event->accept();
}

void DocumentView::updatePages()
{
    QList<int> pagesIndecies = pageIndiciesToRender();

    for(int i = pagesIndecies.first(); i < pagesIndecies.size(); ++i)
    {
        TestRect* page = new TestRect(this);
        page->setParentItem(this);
        page->setY(0);
        page->setWidth(page->implicitWidth());
        page->setHeight(page->getImplicitHeight());

        // PageView* page = new PageView(this, m_bookController, i,
        // m_currentZoom,
        //                               window()->devicePixelRatio());
        // page->setParentItem(this);
        // page->setY(0);
        // int x = page->getImplicitWidth();
        // int y = page->getImplicitHeight();
        // page->setWidth(page->implicitWidth());
        // page->setHeight(page->getImplicitHeight());
    }
}

QList<int> DocumentView::pageIndiciesToRender() const
{
    // This means no page was rendered yet, we have to render the first
    // one to get the height for the next pages
    if(m_pageHeight == 0)
        return { 0 };

    return { 2 };
}

void DocumentView::handleScroll(int deltaY)
{
    deltaY *= -1;  // To get the correct scroll direction

    int maxContentY =
        (m_activePages.first()->getImplicitHeight() * m_currentZoom + 15) *
            m_activePages.size() -
        height();
    if(m_contentY + deltaY > maxContentY)
        m_contentY = maxContentY;
    else if(m_contentY + deltaY < 0)
        m_contentY = 0;
    else
        m_contentY += deltaY;

    update();
}

void DocumentView::handleZoom(int deltaY)
{
    double oldZoom = m_currentZoom;
    m_currentZoom *= (deltaY > 0 ? 1 + m_zoomFactor : 1 - m_zoomFactor);
    m_contentY = computeNewContentYForZoom(oldZoom, m_currentZoom);
    update();
}

double DocumentView::computeNewContentYForZoom(double oldZoom,
                                               double newZoom) const
{
    // We want to move towards the mouse position
    auto mousePos = this->mapFromGlobal(QCursor::pos());
    qreal virtualMouseY = m_contentY + mousePos.y();
    return virtualMouseY * (newZoom / oldZoom) - mousePos.y();
}

void DocumentView::updateImplicitWidth(int newWidth)
{
    setImplicitWidth(newWidth);
}

}  // namespace cpp_elements
