#ifndef MENUBUTTON
#define MENUBUTTON
#include <QPushButton>

class QMenu;
class MenuButton : public QPushButton //继承QPushButton类
{
    Q_OBJECT

public:
    explicit MenuButton(QWidget *parent = 0);
    QMenu *getmenu();

private slots:
    void PushMenu();

private:
    QMenu *menu;
};
#endif // MENUBUTTON

