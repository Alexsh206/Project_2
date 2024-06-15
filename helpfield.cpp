#include "helpfield.h"

HelpField::HelpField()
{
    setFixedSize(600, 50);
    m_text = "Score: 0\nPause - SPACE";
}


void HelpField::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter;
    painter.begin(this);
    painter.drawRect(0, 0, width() - 1, height() - 1);
    painter.setPen(QColor(250, 250, 250));
    painter.setFont(QFont("Arial", 10, 700));
    painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, m_text);

    painter.end();
}

void HelpField::ChangeTextSlot(QString text)
{
    m_text = text;
    repaint();

}
