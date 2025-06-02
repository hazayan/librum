#pragma once
#include <QQuickItem>

class TestRect : public QQuickItem
{
public:
    TestRect(QObject* parent);

    int getImplicitWidth();
    int getImplicitHeight();

protected:
    QSGNode *updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData *);

private:
    int m_implicitWidth = 450;
    int m_implicitHeight = 600;
};
