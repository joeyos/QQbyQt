
#include "menubutton.h"
#include <QMenu>

MenuButton::MenuButton(QWidget *parent):
    QPushButton(parent)
{
    menu = new QMenu(this);
    connect(this,SIGNAL(clicked()),this,SLOT(PushMenu()));
}

QMenu *MenuButton::getmenu()
{
    return menu;
}

void MenuButton::PushMenu()
{
    QPoint pos;
    int y = pos.y();
    pos.setY(y+this->geometry().height());
    menu->exec(this->mapToGlobal(pos));
}
