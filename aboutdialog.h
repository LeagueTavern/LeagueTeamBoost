#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QShowEvent>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

protected:
    void showEvent(QShowEvent *e);

private:
    Ui::AboutDialog *ui;

    int m_DwmWidth;
};

#endif // ABOUTDIALOG_H
