#pragma once
#include <QList>
#include <QQuickItem>
#include <QSGSimpleRectNode>
#include "page_view.hpp"
#include "presentation_export.hpp"

namespace cpp_elements
{

struct MyItem
{
    QColor color;
    int width;
    int height;
};

class PRESENTATION_EXPORT DocumentView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(adapters::IBookController* bookController WRITE setBookController
                   CONSTANT)

public:
    DocumentView();
    void setBookController(adapters::IBookController* newBookController);

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void updatePages();
    QList<int> pageIndiciesToRender() const;
    Q_INVOKABLE void updateImplicitWidth(int newWidth);
    void handleScroll(int deltaY);
    void handleZoom(int deltaY);
    double computeNewContentYForZoom(double oldZoom, double newZoom) const;

    adapters::IBookController* m_bookController = nullptr;
    QList<PageView*> m_activePages;
    int m_pageHeight = 0;
    int m_contentY = 0;
    bool m_controlPressed = false;
    double m_currentZoom = 1;
    double m_zoomFactor = 0.13;
};

}  // namespace cpp_elements
