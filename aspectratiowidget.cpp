#include "aspectratiowidget.h"
#include <QLayout>
#include <QVBoxLayout>
#include <QResizeEvent>

AspectRatioWidget::AspectRatioWidget(QWidget *childWidget, QWidget *parent)
    : QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    // Layout is needed to properly integrate with Qt's layout system and size hints
    // Actual positioning is done manually in resizeEvent() to maintain aspect ratio and centering
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    if (childWidget) setContentWidget(childWidget);
}

void AspectRatioWidget::setContentWidget(QWidget *w)
{
    if (m_content) {
        layout()->removeWidget(m_content);
        m_content->setParent(nullptr);
    }
    m_content = w;
    if (m_content) {
        m_content->setParent(this);
        m_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // Note: Manual geometry positioning in resizeEvent handles actual placement
        layout()->addWidget(m_content);
    }
    updateGeometry();
}

void AspectRatioWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_content) return;
    QRect r = contentsRect();
    int side = qMin(r.width(), r.height());
    int x = r.x() + (r.width() - side) / 2;
    int y = r.y() + (r.height() - side) / 2;
    m_content->setGeometry(x, y, side, side);
}
