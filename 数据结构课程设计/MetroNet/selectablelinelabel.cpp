#include "selectablelinelabel.h"
#include "mainwindow.h"

SelectableLineLabel::SelectableLineLabel(MetroLine* line, QGraphicsItem* parent)
    : QGraphicsTextItem(line->name, parent), m_line(line), m_selected(false)
{
    setDefaultTextColor(line->color);
    m_originalColor = line->color;
    setAcceptHoverEvents(true);
}

void SelectableLineLabel::setSelected(bool selected)
{
    m_selected = selected;

    // 设置边框属性
    if (selected) {
        // 选中时添加边框
        QTextDocument* doc = document();
        QTextFrameFormat frameFormat = doc->rootFrame()->frameFormat();
        frameFormat.setBorder(2); // 边框宽度
        frameFormat.setBorderBrush(QBrush(Qt::yellow)); // 边框颜色
        frameFormat.setPadding(2); // 内边距
        doc->rootFrame()->setFrameFormat(frameFormat);
    } else {
        // 取消选中时移除边框
        QTextDocument* doc = document();
        QTextFrameFormat frameFormat = doc->rootFrame()->frameFormat();
        frameFormat.setBorder(0); // 无边框
        doc->rootFrame()->setFrameFormat(frameFormat);
    }

    update();
}

void SelectableLineLabel::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    emit clicked(m_line);
}

void SelectableLineLabel::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    // 鼠标悬停时改变光标形状
    setCursor(Qt::PointingHandCursor);
}

void SelectableLineLabel::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    // 恢复默认光标
    unsetCursor();
}
