#include "serverwindow.h"
#include "ui_serverwindow.h"
#include <QTimer>
#include <QMenu>
#include <QMessageBox>

serverwindow::serverwindow(std::shared_ptr<Database> dbPtr, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::serverwindow), m_dbPtr(dbPtr)
{
    ui->setupUi(this);

    //remove notification on common/private chat tab change
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [=](int index)
        {
            QString label = ui->tabWidget->tabText(index);
            ui->tabWidget->setTabText(index, label.remove('*'));
        });

    //Timer func
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &serverwindow::updateInfo);
    timer->start(10);

    //Ban and Block RMB func
    connect(ui->userListWidget, &QListWidget::customContextMenuRequested,
            this, [this](const QPoint &pos) {
                QListWidgetItem *item = ui->userListWidget->itemAt(pos);
                if (!item)
                    return;

                QMenu menu(this);

                QAction *act1;
                if (m_dbPtr->checkIsBanned(item->text().toStdString()))
                    {
                    act1 = menu.addAction(tr("Unban"));
                    } else
                    {
                    act1 = menu.addAction(tr("Ban"));
                    }
                QAction *act2;
                    if (m_dbPtr->checkIsBlocked(item->text().toStdString()))
                    {
                        act2 = menu.addAction(tr("Unblock"));
                    } else
                    {
                        act2 = menu.addAction(tr("Block"));
                    }

                QAction *chosen = menu.exec(ui->userListWidget->viewport()->mapToGlobal(pos));

                if ((chosen == act1) && (!m_dbPtr->checkIsBanned(item->text().toStdString()))) {
                    m_dbPtr->banUser(item->text().toStdString());
                    QMessageBox::information(this, tr("Info"), tr("User ") + item->text() + tr(" is banned"));
                } else if ((chosen == act1) && (m_dbPtr->checkIsBanned(item->text().toStdString())))
                {
                    m_dbPtr->unbanUser(item->text().toStdString());
                    QMessageBox::information(this, "Info", "User " + item->text() + " is unbanned");
                } else if ((chosen == act2) && (!m_dbPtr->checkIsBlocked(item->text().toStdString())))
                {
                    m_dbPtr->blockUser(item->text().toStdString());
                    QMessageBox::information(this, "Info", "User " + item->text() + " is blocked");
                } else if ((chosen == act2) && (m_dbPtr->checkIsBlocked(item->text().toStdString())))
                {
                    m_dbPtr->unblockUser(item->text().toStdString());
                    QMessageBox::information(this, "Info", "User " + item->text() + " is unblocked");
                }
            });
}

serverwindow::~serverwindow()
{
    delete ui;
}

void serverwindow::updateInfo()
{
//user list
    auto userList = m_dbPtr->getUserList();
    for(auto user : userList)
    {
        if(ui->userListWidget->findItems(QString::fromStdString(user), Qt::MatchExactly).isEmpty())
        ui->userListWidget->addItem(QString::fromStdString(user));
    }

//common chat
    auto chatMessages = m_dbPtr->getChatMessages();
    QString chat;
    for(const auto &msg : chatMessages)
    {
        chat.append(QString::fromStdString(msg) + "\n");
    }
    if (ui->commonChatBrowser->toPlainText() != chat)
    {
        ui->commonChatBrowser->setText(chat);

        //add notification on private chat tab
        if ((!ui->tabWidget->tabText(0).endsWith("*")) && (1 == ui->tabWidget->currentIndex()))
        {
            ui->tabWidget->setTabText(0, ui->tabWidget->tabText(0) + "*");
            QApplication::beep();
        }
    }

    chat.clear();

//private chat
    auto privateMessages = m_dbPtr->getPrivateMessage();
    for(const auto &msg : privateMessages)
    {
        QString prefix;
        if(QString::fromStdString(msg.getSender()) == QString::fromStdString(m_dbPtr->getUserName(msg.getDest())))
        {
            prefix = "<" + QString::fromStdString(msg.getSender()) + "> " + tr("sent self message") + ": ";
        }
        else
        {
            prefix = "<" + QString::fromStdString(msg.getSender()) + "> " + tr("says to ") + "<" + QString::fromStdString(m_dbPtr->getUserName(msg.getDest())) + ">" + ": ";
        }
        chat.append("[" + QString::fromStdString(msg.getTimestamp()) + "] " + prefix + QString::fromStdString(msg.getText()) + "\n");
    }


    if (ui->privateChatBrowser->toPlainText() != chat)
    {
        ui->privateChatBrowser->setText(chat);

        //add notification on private chat tab
        if ((!ui->tabWidget->tabText(1).endsWith("*")) && (0 == ui->tabWidget->currentIndex()))
        {
            ui->tabWidget->setTabText(1, ui->tabWidget->tabText(1) + "*");
            QApplication::beep();
        }
    }
}
