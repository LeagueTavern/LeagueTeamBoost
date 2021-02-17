#include "clickablelabel.h"


ClickableLabel::ClickableLabel(const QString &text, QWidget *parent)
    : QLabel(parent)
{
    setText(text);
}

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent)
{
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
}
