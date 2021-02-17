#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QMouseEvent>
#include "httpsmanager.h"
#include "aboutdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent* e);

private:
    void connectSlots();
    void formatPath(QString path);

private slots:
    void selectGamePath();
    void getGamePath();

    QString getBaseUrl();

    void unlockTeamBoost();
    void getTeamBoostState();

private:
    Ui::MainWindow *ui;

    bool m_bDragging;          // 是否正在拖动
    QPoint m_poStartPosition;  // 拖动开始前的鼠标位置
    QPoint m_poFramePosition;  // 窗体的原始位置

    QString m_gamePath;
    HttpsManager *m_httpsManager;

    AboutDialog *m_aboutDialog;
};
#endif // WIDGET_H
