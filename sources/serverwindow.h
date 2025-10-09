#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include "Database.h"

namespace Ui {
class serverwindow;
}

class serverwindow : public QWidget
{
    Q_OBJECT

public:
    explicit serverwindow(std::shared_ptr<Database> dbPtr, QWidget *parent = nullptr);
    ~serverwindow();

private slots:
    void updateInfo();

private:
    Ui::serverwindow *ui;
    std::shared_ptr<Database> m_dbPtr;
};

#endif // SERVERWINDOW_H
