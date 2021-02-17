#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <dwmapi.h>
#include <QDesktopServices>
#include <QUrl>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
    , m_DwmWidth(0)
{
    ui->setupUi(this);

    /* 窗口风格 */
    this->setFixedSize(this->size());
    this->setWindowFlags((this->windowFlags() & ~Qt::WindowMinMaxButtonsHint) | Qt::FramelessWindowHint);

    /* 窗口阴影 */
    BOOL bEnable = false;
    ::DwmIsCompositionEnabled(&bEnable);
    if (bEnable) {
        m_DwmWidth = 7;
        DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
        ::DwmSetWindowAttribute((HWND)winId(), DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
        MARGINS margins = { -1 };
        ::DwmExtendFrameIntoClientArea((HWND)winId(), &margins);
    }

    /* 信号槽 */
    connect(ui->btn_ok, &QPushButton::clicked, this, &AboutDialog::hide);
    connect(ui->btn_github, &QPushButton::clicked, this, [=] () {
        QDesktopServices::openUrl(QUrl("https://github.com/LeaguePrank/LeagueTeamBoost"));
    });
    connect(ui->btn_bilibili, &QPushButton::clicked, this, [=] () {
        QDesktopServices::openUrl(QUrl("https://space.bilibili.com/248303677"));
        QDesktopServices::openUrl(QUrl("https://space.bilibili.com/14671179"));
    });
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
    int x = parentWidget()->x() + (parentWidget()->width() / 2);
    int y = parentWidget()->y() + (parentWidget()->height() / 2);
    x -= this->width() / 2 + m_DwmWidth;  // m_DwmWidth = 7 可能是dwm的宽度？需要再研究
    y -= this->height() / 2;
    move(x, y);
}
