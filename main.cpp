#include <QApplication>
#include <QMainWindow>
#include <QDebug>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("自定义节点编辑器");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("CustomNodeEditor");

    qDebug() << "应用程序启动...";

    try {
        MainWindow window;
        window.show();

        qDebug() << "主窗口创建成功";

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "应用程序启动失败:" << e.what();
        return 1;
    } catch (...) {
        qCritical() << "应用程序启动失败: 未知异常";
        return 1;
    }
}