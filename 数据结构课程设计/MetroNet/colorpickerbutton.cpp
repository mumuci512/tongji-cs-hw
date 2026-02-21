#include "colorpickerbutton.h"
#include <QStyleOption>
#include <QPainter>

ColorPickerButton::ColorPickerButton(QWidget *parent)
    : QPushButton(parent), m_color(Qt::red)
{
    updateButtonColor();
    connect(this, &QPushButton::clicked, this, &ColorPickerButton::pickColor);
}

QColor ColorPickerButton::color() const
{
    return m_color;
}

void ColorPickerButton::setColor(const QColor &color)
{
    if (m_color != color)
    {
        m_color = color;
        updateButtonColor();
        emit colorChanged(color);
    }
}

void ColorPickerButton::pickColor()
{
    QColor newColor = QColorDialog::getColor(m_color, this, "选择颜色");
    if (newColor.isValid())
    {
        setColor(newColor);
    }
}

void ColorPickerButton::updateButtonColor()
{
    // 使用类名选择器，确保样式表只应用于ColorPickerButton类，不要应用于子组件QColorDialog
    QString style = QString("ColorPickerButton { "
                           "background-color: %1; "
                           "color: %2; "
                           "border: 1px solid gray; "
                           "}")
                   .arg(m_color.name())
                   .arg(m_color.lightness() > 128 ? "black" : "white");
    setStyleSheet(style);
    setText(m_color.name());

    update();
}
