#ifndef SELECTABLELINELABEL_H
#define SELECTABLELINELABEL_H
#include <QGraphicsTextItem>
#include <QTextFrame>
#include <QTextDocument>

class MetroLine;

class SelectableLineLabel : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit SelectableLineLabel(MetroLine* line, QGraphicsItem* parent = nullptr);

    MetroLine* line() const { return m_line; }
    bool isSelected() const { return m_selected; }
    void setSelected(bool selected);

signals:
    void clicked(MetroLine* line);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    MetroLine* m_line;
    bool m_selected;
    QColor m_originalColor;
};

#endif // SELECTABLELINELABEL_H
