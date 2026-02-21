#pragma execution_character_set("utf-8")
#ifndef COLORPICKERBUTTON_H
#define COLORPICKERBUTTON_H

#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QString>
#include <QWidget>

class ColorPickerButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit ColorPickerButton(QWidget *parent = nullptr);

    QColor color() const;

public slots:
    void setColor(const QColor &color);
    void pickColor();

signals:
    void colorChanged(const QColor &color);

private:
    void updateButtonColor();

    QColor m_color;
};

#endif // COLORPICKERBUTTON_H
