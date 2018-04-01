#include "widget.h"
#include "ui_widget.h"
#include<QDebug>

#include<QUdpSocket>
#include<QHostInfo>
#include<QMessageBox>
#include<QScrollBar>
#include<QDateTime>
#include<QNetworkInterface>
#include<QProcess>

#include "tcpserver.h"
#include "tcpclient.h"
#include <QFileDialog>

#include <QColorDialog>

#include <QPropertyAnimation>
#include <QMouseEvent>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //==========================
    udpSocket = new QUdpSocket(this);
    port = 45454;
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    sendMessage(NewParticipant);

    server = new TcpServer(this);
    connect(server, SIGNAL(sendFileName(QString)), this, SLOT(getFileName(QString)));

    connect(ui->messageTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),this, SLOT(currentFormatChanged(const QTextCharFormat)));

    setWindowFlags(Qt::FramelessWindowHint);//无边框
    setAttribute(Qt::WA_TranslucentBackground);//背景透明
    //-------------------------------
    ui->sendButton->setFocus();    //设置默认焦点
    ui->sendButton->setShortcut( QKeySequence::InsertParagraphSeparator );  //设置快捷键为键盘的“回车”键
    ui->sendButton->setShortcut(Qt::Key_Enter);  //设置快捷键为enter键
    ui->sendButton->setShortcut(Qt::Key_Return); //设置快捷键为小键盘上的enter键
    ui->sendButton->setFocus();    //设置默认焦点
    ui->sendButton->setDefault(true);  //设置默认按钮，设置了这个属性，当用户按下回车的时候，就会按下该按钮
    //-------------------------------
    //this->setWindowFlags(Qt::FramelessWindowHint);//==============shadow============
    //this->setAttribute(Qt::WA_TranslucentBackground);
    //============================================================shadow============
    ui->messageBrowser->verticalScrollBar()->setStyleSheet(
        "QScrollBar:vertical{width:8px;background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;padding-top:9px;padding-bottom:9px;}"//留9px给上下箭头
        "QScrollBar::handle:vertical{width:8px;background:rgba(0,0,0,25%);border-radius:4px;min-height:20;}"//上下设置为椭圆
        "QScrollBar::handle:vertical:hover{width:8px;background:rgba(0,0,0,50%);border-radius:4px;min-height:20;}"//鼠标悬浮颜色变深
        "QScrollBar::add-line:vertical{height:9px;width:8px;border-image:url(:/images/bottom.png);subcontrol-position:bottom;}"//下箭头
        "QScrollBar::sub-line:vertical{height:9px;width:8px;border-image:url(:/images/top.png);subcontrol-position:top;}"//上箭头
        "QScrollBar::add-line:vertical:hover{height:7px;width:6px;border-image:url(:/images/bottom.png);border:1px;subcontrol-position:bottom;}"//鼠标悬浮下箭头
        "QScrollBar::sub-line:vertical:hover{height:7px;width:6px;border-image:url(:/images/top.png);border:1px;subcontrol-position:top;}"//鼠标悬浮上箭头
        "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:rgba(0,0,0,10%);border-radius:4px;}");//滚动时部分
    ui->messageTextEdit->verticalScrollBar()->setStyleSheet(
    "QScrollBar:vertical{width:8px;background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;padding-top:9px;padding-bottom:9px;}"//留9px给上下箭头
    "QScrollBar::handle:vertical{width:8px;background:rgba(0,0,0,25%);border-radius:4px;min-height:20;}"//上下设置为椭圆
    "QScrollBar::handle:vertical:hover{width:8px;background:rgba(0,0,0,50%);border-radius:4px;min-height:20;}"//鼠标悬浮颜色变深
    "QScrollBar::add-line:vertical{height:9px;width:8px;border-image:url(:/images/bottom.png);subcontrol-position:bottom;}"//下箭头
    "QScrollBar::sub-line:vertical{height:9px;width:8px;border-image:url(:/images/top.png);subcontrol-position:top;}"//上箭头
    "QScrollBar::add-line:vertical:hover{height:7px;width:6px;border-image:url(:/images/bottom.png);border:1px;subcontrol-position:bottom;}"//鼠标悬浮下箭头
    "QScrollBar::sub-line:vertical:hover{height:7px;width:6px;border-image:url(:/images/top.png);border:1px;subcontrol-position:top;}"//鼠标悬浮上箭头
    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:rgba(0,0,0,10%);border-radius:4px;}");//滚动时部分
    ui->userTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//表格只读
    ui->userTableWidget->setItemDelegate(new NoFocusDelegate());//去掉表格虚框
    ui->userTableWidget->setTextElideMode(Qt::ElideRight);//防止文本过长显示右边
    /* 显示菜单 */
    QMenu * menu = ui->menuButton->getmenu();
    b1 = new QAction(QIcon(":/images/menu.png"), tr("&星球皮肤"), this);
    b2 = new QAction(QIcon(":/images/menu.png"), tr("&绿草皮肤"), this);
    b3 = new QAction(QIcon(":/images/menu.png"), tr("&唯美皮肤"), this);
    b4 = new QAction(QIcon(":/images/menu.png"), tr("&秋季皮肤"), this);
    b = new QAction(QIcon(":/images/menu.png"), tr("&原始皮肤"), this);
    menu->addAction(b1);
    menu->addAction(b2);
    menu->addAction(b3);
    menu->addAction(b4);
    menu->addAction(b);
    connect(b1,SIGNAL(triggered(bool)),this,SLOT(bg1()));
    connect(b2,SIGNAL(triggered(bool)),this,SLOT(bg2()));
    connect(b3,SIGNAL(triggered(bool)),this,SLOT(bg3()));
    connect(b4,SIGNAL(triggered(bool)),this,SLOT(bg4()));
    connect(b,SIGNAL(triggered(bool)),this,SLOT(bg()));
    ui->messageTextEdit->installEventFilter(this);//回车键发消息监听
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);//任务栏使程序最小化
}

Widget::~Widget()
{
    delete ui;
}
//////////////////////////////////////////////////////
//发送消息
void Widget::sendMessage(MessageType type,QString serverAddress)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);//======out
    QString localHostName=QHostInfo::localHostName();
    QString address=getIP();
    out<<type<<getUserName()<<localHostName;
    switch(type)
    {
    case Message:
        if(ui->messageTextEdit->toPlainText()=="")
        {
            QMessageBox::warning(0,tr("警告"),tr("发送内容不为空"),QMessageBox::Ok);
            return;
        }
        out<<address<<getMessage();
        ui->messageBrowser->verticalScrollBar()->setValue(ui->messageBrowser->verticalScrollBar()->maximum());
        break;
    case NewParticipant:
        out<<address;
        break;
    case Participantleft:
        break;
    case FileName : {
        int row = ui->userTableWidget->currentRow();
        QString clientAddress = ui->userTableWidget->item(row, 2)->text();
        out << address << clientAddress << fileName;
        break;
    }
    case Refuse :
        out << serverAddress;
        break;
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);//UDP广播
}
//接收消息并处理
void Widget::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);//=======in
        int MessageType;
        in>>MessageType;
        QString userName,localHostName,ipAddress,message;
        QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        switch(MessageType)
        {
        case Message:
            in>>userName>>localHostName>>ipAddress>>message;
            ui->messageBrowser->setTextColor(Qt::blue);
            ui->messageBrowser->setCurrentFont(QFont("黑体",12));
            ui->messageBrowser->append("["+userName+"]"+time);
            ui->messageBrowser->append(message);
            break;
        case NewParticipant:
            in>>userName>>localHostName>>ipAddress;
            newParticipant(userName,localHostName,ipAddress);
            break;
        case Participantleft:
            in>>userName>>localHostName;
            participantleft(userName,localHostName,time);
            break;
        case FileName:
        {
            in >> userName >> localHostName >> ipAddress;
            QString clientAddress, fileName;
            in >> clientAddress >> fileName;
            hasPendingFile(userName, ipAddress, clientAddress, fileName);
            break;
        }
        case Refuse:
        {
            in >> userName >> localHostName;
            QString serverAddress;
            in >> serverAddress;
            QString ipAddress = getIP();
            if(ipAddress == serverAddress)
            {
                server->refused();
            }
            break;
        }
        }
    }
}
//用户进入聊天
void Widget::newParticipant(QString userName,QString localHostName,QString ipAddress)
{
    bool isEmpty=ui->userTableWidget->findItems(localHostName,Qt::MatchExactly).isEmpty();
    if(isEmpty)
    {
        QTableWidgetItem *user=new QTableWidgetItem(userName);
        QTableWidgetItem *host=new QTableWidgetItem(localHostName);
        QTableWidgetItem *ip=new QTableWidgetItem(ipAddress);
        user->setTextAlignment(Qt::AlignCenter);//========文字居中显示
        host->setTextAlignment(Qt::AlignCenter);
        ip->setTextAlignment(Qt::AlignCenter);
        ui->userTableWidget->insertRow(0);
        ui->userTableWidget->setItem(0,0,user);
        ui->userTableWidget->setItem(0,1,host);
        ui->userTableWidget->setItem(0,2,ip);

        ui->messageBrowser->setTextColor(Qt::gray);
        ui->messageBrowser->setCurrentFont(QFont("黑体",10));
        ui->messageBrowser->append(tr("%1 在线！").arg(userName));
        ui->userNumLabel->setText(tr("在线人数：%1").arg(ui->userTableWidget->rowCount()));
        sendMessage(NewParticipant);
    }
}
//用户离开聊天
void Widget::participantleft(QString userName, QString localHostName, QString time)
{
    int rowNum=ui->userTableWidget->findItems(localHostName,Qt::MatchExactly).first()->row();
    ui->userTableWidget->removeRow(rowNum);
    ui->messageBrowser->setTextColor(Qt::gray);
    ui->messageBrowser->setCurrentFont(QFont("黑体",10));
    ui->messageBrowser->append(tr("%1 于 %2 离开！").arg(userName).arg(time));
    ui->userNumLabel->setText(tr("在线人数：%1").arg(ui->userTableWidget->rowCount()));
}
QString Widget::getIP()
{/*
    QList<QHostAddress> list=QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol)
            return address.toString();
    }*/
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info=QHostInfo::fromName(localHostName);
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}
QString Widget::getUserName()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment=QProcess::systemEnvironment();
    foreach(QString string,envVariables)
    {
        int index=environment.indexOf(QRegExp(string));
        if(index!=-1)
        {
            QStringList stringlList=environment.at(index).split('=');
            if(stringlList.size()==2)
            {
                return stringlList.at(1);
                break;
            }
        }
    }
    return "unknown";
}
QString Widget::getMessage()
{
    QString msg=ui->messageTextEdit->toHtml();
    ui->messageTextEdit->clear();
    ui->messageTextEdit->setFocus();
    return msg;
}
void Widget::on_sendButton_clicked()
{
    sendMessage(Message);
}
//获取要发送的文件名
void Widget::getFileName(QString name)
{
    fileName = name;
    sendMessage(FileName);
}
//传输文件按钮
void Widget::on_sendToolButton_clicked()
{
    if(ui->userTableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0, tr("选择用户"),
                       tr("请先从用户列表选择要传送的用户！"), QMessageBox::Ok);
        return;
    }
    server->show();
    server->initServer();
}
//判断是否接收文件,对话框
void Widget::hasPendingFile(QString userName, QString serverAddress,QString clientAddress, QString fileName)
{
    QString ipAddress = getIP();
    if(ipAddress == clientAddress)
    {
        int btn = QMessageBox::information(this,tr("接受文件"),
                                           tr("来自%1(%2)的文件：%3,是否接收？")
                                           .arg(userName).arg(serverAddress).arg(fileName),
                                           QMessageBox::Yes,QMessageBox::No);
        if (btn == QMessageBox::Yes) {
            QString name = QFileDialog::getSaveFileName(0,tr("保存文件"),fileName);
            if(!name.isEmpty())
            {
                tcpclient *client = new tcpclient(this);
                client->setFileName(name);
                client->setHostAddress(QHostAddress(serverAddress));
                client->show();
            }
        } else {
            sendMessage(Refuse, serverAddress);
        }
    }
}

void Widget::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->messageTextEdit->setCurrentFont(f);
    ui->messageTextEdit->setFocus();
}
void Widget::on_sizeComboBox_currentIndexChanged(QString size)
{
    ui->messageTextEdit->setFontPointSize(size.toDouble());
    ui->messageTextEdit->setFocus();
}
void Widget::on_boldToolButton_clicked(bool checked)
{//粗体，按钮属性设置为checkable
    if(checked)
        ui->messageTextEdit->setFontWeight(QFont::Bold);
    else
        ui->messageTextEdit->setFontWeight(QFont::Normal);
    ui->messageTextEdit->setFocus();
}
void Widget::on_italicToolButton_clicked(bool checked)
{//斜体，checkable
    ui->messageTextEdit->setFontItalic(checked);
    ui->messageTextEdit->setFocus();
}
void Widget::on_underlineToolButton_clicked(bool checked)
{//下划线，checkable
    ui->messageTextEdit->setFontUnderline(checked);
    ui->messageTextEdit->setFocus();
}
void Widget::on_colorToolButton_clicked()
{
    color = QColorDialog::getColor(color, this);
    if (color.isValid()) {
        ui->messageTextEdit->setTextColor(color);
        ui->messageTextEdit->setFocus();
    }
}
//保存聊天记录
void Widget::on_saveToolButton_clicked()
{
    if (ui->messageBrowser->document()->isEmpty()) {
        QMessageBox::warning(0, tr("警告"), tr("聊天记录为空，无法保存！"), QMessageBox::Ok);
    } else {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("保存聊天记录"), tr("聊天记录"), tr("文本(*.txt);;All File(*.*)"));
        if(!fileName.isEmpty())
            saveFile(fileName);
    }
}
bool Widget::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("保存文件"),
                             tr("无法保存文件 %1:\n %2").arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << ui->messageBrowser->toPlainText();

    return true;
}
void Widget::on_clearToolButton_clicked()
{
    ui->messageBrowser->clear();
}
void Widget::currentFormatChanged(const QTextCharFormat &format)
{//鼠标点击不同大小文字，转换到对应状态
    ui->fontComboBox->setCurrentFont(format.font());

    if (format.fontPointSize() < 9) {
        ui->sizeComboBox->setCurrentIndex(3);
    } else {
        ui->sizeComboBox->setCurrentIndex( ui->sizeComboBox
                                          ->findText(QString::number(format.fontPointSize())));
    }
    ui->boldToolButton->setChecked(format.font().bold());
    ui->italicToolButton->setChecked(format.font().italic());
    ui->underlineToolButton->setChecked(format.font().underline());
    color = format.foreground().color();
}

void Widget::on_exitButton_clicked()
{
    close();
}
// 关闭事件
void Widget::closeEvent(QCloseEvent *e)
{
    sendMessage(Participantleft);
    QWidget::closeEvent(e);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if(this->width() > 45 && this->height() > 45)
        drawWindowShadow(p);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    p.drawRoundedRect(QRect(5,5,this->width() - 10,this->height() - 10),2.0f,2.0f);
}
void Widget::drawWindowShadow(QPainter &p)
{
    QList<QPixmap> pixmaps;
    pixmaps.append(QPixmap(":/images/shadow_left.png"));
    pixmaps.append(QPixmap(":/images/shadow_right.png"));
    pixmaps.append(QPixmap(":/images/shadow_top.png"));
    pixmaps.append(QPixmap(":/images/shadow_bottom.png"));
    pixmaps.append(QPixmap(":/images/shadow_corner1.png"));
    pixmaps.append(QPixmap(":/images/shadow_corner2.png"));
    pixmaps.append(QPixmap(":/images/shadow_corner3.png"));
    pixmaps.append(QPixmap(":/images/shadow_corner4.png"));

    p.drawPixmap(0,0,5,5,pixmaps[4]);
    p.drawPixmap(this->width() - 5,0,5,5,pixmaps[5]);
    p.drawPixmap(0,this->height() - 5,5,5,pixmaps[6]);
    p.drawPixmap(this->width() - 5,this->height() - 5,5,5,pixmaps[7]);

    p.drawPixmap(0,5,5,this->height() - 10,pixmaps[0].scaled(5,this->height() - 10));
    p.drawPixmap(this->width() - 5,5,5,this->height() - 10,pixmaps[1].scaled(5,this->height() - 10));
    p.drawPixmap(5,0,this->width() - 10,5,pixmaps[2].scaled(this->width() - 10,5));
    p.drawPixmap(5,this->height() - 5,this->width() - 10,5,pixmaps[3].scaled(this->width() - 10,5));
}
void Widget::on_closeButton_clicked()
{
    /* 慢慢消失的效果 */
    QPropertyAnimation *animation = new QPropertyAnimation(this,"windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation,SIGNAL(finished()),this,SLOT(close()));//效果显示完后关闭
}

void Widget::on_minButton_clicked()
{
    this->showMinimized();//最小化
}
void Widget::on_maxButton_clicked()
{
    //this->showMaximized();//最大化(未完成)
    QMessageBox::warning(0,tr("提示"),tr("测试功能"),QMessageBox::Ok);
}
void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
    //记录窗口此时的坐标
    StartPos = event->globalPos() - this->frameGeometry().topLeft();
    }
}
//---------------------虚框------------------------------
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    //去表格虚框
    QPoint EndPos;
    if ( event->buttons() == Qt::LeftButton )
    {
    EndPos = event->globalPos() - StartPos;
    this->move(EndPos);
    }
}
void NoFocusDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption(option);
    if (itemOption.state & QStyle::State_HasFocus)
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
    QStyledItemDelegate::paint(painter, itemOption, index);
}
//----------------------------------------------------------
void Widget::bg1()
{
    ui->stackedWidget->setStyleSheet("QStackedWidget {background-image: url(:/images/background1.jpg);}");
}
void Widget::bg2()
{
    ui->stackedWidget->setStyleSheet("QStackedWidget {background-image: url(:/images/background2.jpg);}");
}
void Widget::bg3()
{
    ui->stackedWidget->setStyleSheet("QStackedWidget {background-image: url(:/images/background3.jpg);}");
}
void Widget::bg4()
{
    ui->stackedWidget->setStyleSheet("QStackedWidget {background-image: url(:/images/background4.jpg);}");
}
void Widget::bg()
{
    ui->stackedWidget->setStyleSheet("QStackedWidget {background-image: url(:/images/background.jpg);}");
}
//----------------回车键发消息---------------------
//void Widget::on_messageTextEdit_textChanged()
//{
//}
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->messageTextEdit)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast <QKeyEvent *>(event);
            if(keyEvent->key()==Qt::Key_Return)
            {
                qDebug()<<"int envent filter";
                emit sendMessage(Message);
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}
//QlineEdit直接用returnPressed()即可
//connect(ui->lineEdit, SIGNAL(returnPressed()), ui->pushButton, SIGNAL(clicked()), Qt::UniqueConnection);
//----------------------------------------------------------------------------
