#include "test_rect.hpp"
#include <QSGSimpleRectNode>

TestRect::TestRect(QObject* parent)
{
    setParent(parent);
    setFlag(QQuickItem::ItemHasContents, true);
}

int TestRect::getImplicitWidth()
{
    return m_implicitWidth;
}

int TestRect::getImplicitHeight()
{
    return m_implicitHeight;
}

QSGNode* TestRect::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    QSGSimpleRectNode* node = static_cast<QSGSimpleRectNode*>(oldNode);
    if(!node)
    {
        node = new QSGSimpleRectNode();
        node->setColor(Qt::green);
        node->setRect(0, 0, m_implicitWidth, m_implicitHeight);
    }

    return node;
}
