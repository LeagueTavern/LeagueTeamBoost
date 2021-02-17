#include "widget.h"

#include <QApplication>
#include <QFont>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

    QApplication a(argc, argv);

    a.setFont(QFont("Microsoft YaHei"));

    QFile qss(":/resources/stylesheet/style.qss");
    if(qss.open(QFile::ReadOnly)) {
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }

    Widget w;
    w.show();

    return a.exec();
}
