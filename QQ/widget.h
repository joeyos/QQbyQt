#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QTextCharFormat>

#include <windows.h>//shadow
#include <windowsx.h>
#include <QtGui>
#include <QItemDelegate>
#include <QStyledItemDelegate>

#include <QMenu>
#include <QAction>

namespace Ui {
class Widget;
}
//=====枚举标志信息类型，分别为消息、新用户加入1、用户退出、文件名、拒绝接受文件==============
class QUdpSocket;
enum MessageType{Message,NewParticipant,Participantleft,FileName,Refuse};
class TcpServer;
//================================
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    /* AboutpushButton按钮下的菜单 */
    QAction *b1,*b2,*b3,*b4,*b;
//===================
private:
    Ui::Widget *ui;

    QUdpSocket * udpSocket;
    qint16 port;
    QString fileName;
    QString localHostName;
    TcpServer *server;
    QColor color;
    void drawWindowShadow(QPainter &p);
    /* 鼠标拖动窗口 */
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    /* 记录窗口当前的位置 */
    QPoint StartPos;
//-------------------------------------------
protected:
    void sendMessage(MessageType type,QString serverAddress="");
    void newParticipant(QString userName,QString localHostName,QString ipAddress);
    void participantleft(QString userName,QString localHostName,QString time);
    QString getIP();
    QString getUserName();
    QString getMessage();
    void hasPendingFile(QString userName, QString serverAddress,
                        QString clientAddress, QString fileName);
    bool saveFile(const QString& fileName);
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent *);//shadow
    bool eventFilter(QObject *obj, QEvent *event);
//-----------------------------------------
private slots:
    void processPendingDatagrams();
    void getFileName(QString);
    void on_sendButton_clicked();
    void on_sendToolButton_clicked();
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_sizeComboBox_currentIndexChanged(QString);
    void on_boldToolButton_clicked(bool checked);
    void on_italicToolButton_clicked(bool checked);
    void on_underlineToolButton_clicked(bool checked);
    void on_colorToolButton_clicked();
    void currentFormatChanged(const QTextCharFormat &format);
    void on_exitButton_clicked();
    void on_saveToolButton_clicked();
    void on_clearToolButton_clicked();
    //void on_messageTextEdit_textChanged();
    void on_closeButton_clicked();
    void on_minButton_clicked();
    void on_maxButton_clicked();
    void bg();
    void bg1();
    void bg2();
    void bg3();
    void bg4();
};

class NoFocusDelegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};



#endif // WIDGET_H
