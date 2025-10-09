#include "mainwindow.h"
#include "startscreen.h"
#include "ui_mainwindow.h"
#include "serverwindow.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTimer>
#include <QTime>
#include <QMessageBox>

int MainWindow::kInstanceCount = 0;

MainWindow::MainWindow(int userId, QString userName, std::shared_ptr<Database> dbPtr, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_userId(userId),
    m_userName(userName)
{
    ui->setupUi(this);
    kInstanceCount++;

    if(dbPtr)
    {
        m_dbPtr = dbPtr;
    } else
    {
        m_dbPtr = make_shared<Database>();
    }

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateChats);
    timer->start(10);

    //remove notification on common/private chat tab change
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [=](int index)
            {
        QString label = ui->tabWidget->tabText(index);
        ui->tabWidget->setTabText(index, label.remove('*'));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    kInstanceCount--;
    if(kInstanceCount <= 0)
    {
        qApp->exit(0);
    }
}

MainWindow *MainWindow::createClient(std::shared_ptr<Database> dbPtr)
{
    StartScreen s(dbPtr);
    auto result = s.exec();
    if (result == QDialog::Rejected)
    {
        return nullptr;
    }
    auto w = new MainWindow(s.userId(), s.userName(), s.getDatabase());
    w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

void MainWindow::on_messageLineEdit_returnPressed()
{
    on_sendMessageButton_clicked();
}


void MainWindow::on_sendMessageButton_clicked()
{
    m_dbPtr->addChatMessage(QTime::currentTime().toString("HH:mm:ss").toStdString(), m_userName.toStdString(), ui->messageLineEdit->text().toStdString());
    ui->messageLineEdit->clear();
}


void MainWindow::on_privateSendButton_clicked()
{
    QDialog dial(this);
    dial.setModal(true);
    auto l = new QVBoxLayout();
    dial.setLayout(l);
    auto userListWgt = new QListWidget(&dial);
    l->addWidget(userListWgt);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dial);
    l->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dial, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dial, &QDialog::reject);

    auto userList = m_dbPtr->getUserList();
    for(auto &user : userList)
    {
        userListWgt->addItem(QString::fromStdString(user));
    }

    userListWgt->setCurrentRow(0);

    auto result = dial.exec();

    if (result == QDialog::Accepted && userListWgt->currentItem())
    {
        m_dbPtr->addPrivateMessage(QTime::currentTime().toString("HH:mm:ss").toStdString(), m_userName.toStdString(), userListWgt->currentItem()->text().toStdString(), ui->messageLineEdit->text().toStdString());
    }

    ui->messageLineEdit->clear();
}


void MainWindow::on_actionCloseClient_triggered()
{
    this->close();
}


void MainWindow::on_actionOpen_another_client_triggered()
{
    auto w = createClient(m_dbPtr);
    if (w)
    {
    w->show();
    }
}

void MainWindow::updateChats()
{
    //Ban and Block func
    if ((m_dbPtr->checkIsBanned(m_userName.toStdString())) && (ui->label->isVisible()))
    {
        ui->messageLineEdit->hide();
        ui->sendMessageButton->hide();
        ui->privateSendButton->hide();
        ui->label->hide();
        QMessageBox::information(this, tr("Info"), tr("You're banned. You can't send messages"));
    } else if ((!m_dbPtr->checkIsBanned(m_userName.toStdString())) && (ui->label->isHidden()))
    {
        ui->messageLineEdit->show();
        ui->sendMessageButton->show();
        ui->privateSendButton->show();
        ui->label->show();
    }

    if (m_dbPtr->checkIsBlocked(m_userName.toStdString()))
    {
        this->close();

        QMessageBox::information(this, tr("Info"), tr("Your account is blocked"));
    }

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

    auto privateMessages = m_dbPtr->getPrivateMessage();
    for(const auto &msg : privateMessages)
    {
        if(QString::fromStdString(msg.getSender()) != m_userName && msg.getDest() != m_userId)
        {
            continue;
        }
        QString prefix;
        if(m_userName == QString::fromStdString(msg.getSender()) && m_userId == msg.getDest())
        {
            prefix = tr("self message") + ": ";
        }
        else if (m_userName == QString::fromStdString(msg.getSender()))
        {
            prefix = tr("your message to") + QString(" <%1>: ").arg(QString::fromStdString(m_dbPtr->getUserName(msg.getDest())));
        } else
        {
            prefix = "<" + QString::fromStdString(msg.getSender()) + "> " + tr("says to you") + ": ";
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

    //user list update
    auto userList = m_dbPtr->getUserList();
    for(auto &user : userList)
    {
        if(ui->userListWidget->findItems(QString::fromStdString(user), Qt::MatchExactly).isEmpty())
            ui->userListWidget->addItem(QString::fromStdString(user));
    }

}


void MainWindow::on_actionServer_settings_triggered()
{
    serverwindow* sw = new serverwindow(m_dbPtr);
    sw->setAttribute(Qt::WA_DeleteOnClose);
    sw->show();
}

