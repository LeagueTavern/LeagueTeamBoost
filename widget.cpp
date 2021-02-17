#include "widget.h"
#include "ui_widget.h"
#include "gameinfo.h"

#include <dwmapi.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopWidget>

const QString StringProcessName = "LeagueClient.exe";
const QString StringUrlBase = "https://riot:%1@127.0.0.1:%2";
const QString StringUrlTeamBoost = "/lol-champ-select/v1/team-boost/purchase";
const QString StringUrlGetBoost = "/lol-champ-select/v1/team-boost";

BOOL DebugPriviliges = FALSE;
BOOL __stdcall AdjustDebugPriviliges(void)
{
    BOOL bRes = FALSE;

    HANDLE hTok;
    if (DebugPriviliges == FALSE && OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hTok))
    {
        TOKEN_PRIVILEGES tp;
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        bRes = AdjustTokenPrivileges(hTok, FALSE, &tp, NULL, NULL, NULL);

        CloseHandle(hTok);
    }

    return DebugPriviliges = bRes;
}

// 超出长度添加省略号
QString getElidedText(QFont font, int maxWidth, QString str)
{
    if (str.isEmpty())
        return "";
    QFontMetrics fontWidth(font);
    int width = fontWidth.width(str);
    if (width >= maxWidth) {
        str = fontWidth.elidedText(str, Qt::ElideRight, maxWidth);
    }
    return str;
}

Widget::Widget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_httpsManager(new HttpsManager)
    , m_aboutDialog(new AboutDialog(this))
{
    ui->setupUi(this);

    /* 窗口风格 */
    this->setFixedSize(this->size());
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);

    /* 窗口阴影 */
    BOOL bEnable = false;
    ::DwmIsCompositionEnabled(&bEnable);
    if (bEnable) {
        DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
        ::DwmSetWindowAttribute((HWND)winId(), DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
        MARGINS margins = { -1 };
        ::DwmExtendFrameIntoClientArea((HWND)winId(), &margins);
    }

    /* 设置按钮可见 */
    ui->btn_state->setVisible(false);
    ui->btn_unlock->setVisible(false);
    ui->btn_tip->setVisible(true);

    /* 准备工作 */
    connectSlots();

    /* 窗口居中 */
    QPoint pos = QWidget::mapToGlobal(this->pos());
    QRect deskRt = QApplication::desktop()->screenGeometry(pos);
    move((deskRt.width() - this->width()) / 2,
         (deskRt.height() - this->height()) / 2);

    /* 关于窗口 */
    m_aboutDialog->show();
}

Widget::~Widget()
{
    m_aboutDialog->deleteLater();
    delete m_httpsManager;
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        QRect rect = ui->centralwidget->rect();
        if (rect.contains(e->pos())) {
            m_bDragging = true;
            m_poStartPosition = e->globalPos();
            m_poFramePosition = frameGeometry().topLeft();
        }
    }
    QWidget::mousePressEvent(e);
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        if (m_bDragging) {
            QPoint delta = e->globalPos() - m_poStartPosition;
            move(m_poFramePosition + delta);
        }
    }
    QWidget::mouseMoveEvent(e);
}

void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    m_bDragging = false;
    QWidget::mouseReleaseEvent(e);
}

/* 连接信号槽 */
void Widget::connectSlots()
{
    connect(ui->btn_min, &QPushButton::clicked, this, &Widget::showMinimized);
    connect(ui->btn_close, &QPushButton::clicked, this, &Widget::close);

    connect(ui->btn_search, &QPushButton::clicked, this, &Widget::getGamePath);
    connect(ui->btn_folder, &QPushButton::clicked, this, &Widget::selectGamePath);
    connect(ui->btn_tip, &QPushButton::clicked, this, &Widget::selectGamePath);
    connect(ui->btn_state, &QPushButton::clicked, this, &Widget::getTeamBoostState);
    connect(ui->btn_unlock, &QPushButton::clicked, this, &Widget::unlockTeamBoost);

    connect(ui->lbl_about, &ClickableLabel::clicked, this, [=] () { m_aboutDialog->show(); });
}

/* 格式化路径，超出长度变为省略号 */
void Widget::formatPath(QString path)
{
    m_gamePath = path;
    QString str = getElidedText(QFont("Microsoft YaHei", 10), 250, path);
    auto disk = str.at(0).toUpper();
    str.remove(0, 1);
    str.insert(0, disk);
    ui->edit_path->setText(str);

    ui->btn_state->setVisible(true);
    ui->btn_unlock->setVisible(true);
    ui->btn_tip->setVisible(false);
}

/* 手动选择游戏路径 */
void Widget::selectGamePath()
{
    QString gamePath;
    auto dialog = new QFileDialog(this);
    dialog->setWindowTitle(QStringLiteral("选择游戏目录中的 %1").arg(StringProcessName));
    //dialog->setDirectory(qApp->applicationDirPath());
    dialog->setDirectory(R"(C:\Program Files\腾讯游戏\英雄联盟\LeagueClient)");
    dialog->setNameFilter(QStringLiteral("%1 (%1)").arg(StringProcessName));
    dialog->setViewMode(QFileDialog::Detail);

    if (dialog->exec() == QDialog::Accepted) {
        gamePath = dialog->selectedFiles().first();
        gamePath.remove(StringProcessName);
        formatPath(gamePath);
    }

    dialog->deleteLater();
}

/* 使用WMIC自动获取游戏路径 */
void Widget::getGamePath()
{
    AdjustDebugPriviliges();
    auto gamePath = GameInfo::getGamePath();
    if (gamePath.isEmpty()) {
        QMessageBox::warning(this,
                             QStringLiteral("失败!"),
                             QStringLiteral("无法自动获取游戏目录，请手动选择游戏目录。或尝试使用管理员权限重新运行程序。\n"
                                            "- 国服 LeagueClient.exe 在游戏根目录的LeagueClient目录下。"));
        return;
    }
    formatPath(gamePath);
}

/* 根据游戏路径读取lockfile并组成api基本的url */
QString Widget::getBaseUrl()
{
    QString url;
    if (m_gamePath.isEmpty()) {
        QMessageBox::warning(this,
                             QStringLiteral("失败!"), QStringLiteral("请先 获取/选择 游戏路径!"));
        return url;
    }

    auto lockfile = GameInfo::getLockFile(m_gamePath);
    if (lockfile.port.isEmpty() || lockfile.token.isEmpty()) {
        QMessageBox::warning(this,
                             QStringLiteral("失败!"), QStringLiteral("lockfile读取失败!"));
        return url;
    }
    url = StringUrlBase.arg(lockfile.token).arg(lockfile.port);

    return url;
}

/* 解锁战斗福利 */
void Widget::unlockTeamBoost()
{
    auto url = getBaseUrl();
    if (url.isEmpty())
        return;
    url += StringUrlTeamBoost;

    QString string = m_httpsManager->post(url, "");
    if (string.isEmpty() || string.contains("errorCode")) {
        QMessageBox::critical(this, QStringLiteral("失败!"), QStringLiteral("请确保当前选择的客户端处于无限火力模式的选人阶段再使用此软件!"));
    }
}

/* 判断当前是否支持解锁战斗福利 */
void Widget::getTeamBoostState()
{
    auto url = getBaseUrl();
    if (url.isEmpty())
        return;
    url += StringUrlGetBoost;

    QString string = m_httpsManager->get(url);
    if (string.isEmpty() || string.contains("errorCode")) {
        QMessageBox::critical(this, QStringLiteral("失败!"), QStringLiteral("当前模式不支持解锁战斗福利!"));
    } else {
        QMessageBox::critical(this, QStringLiteral("成功!"), QStringLiteral("当前模式支持解锁战斗福利!"));
    }
}
