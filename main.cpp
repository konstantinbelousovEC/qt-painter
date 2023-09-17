// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QApplication>
#include "./include/main-window.h"
#include "./include/welcome-dialog.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

#if 0
    WelcomeDialog dialog;
    if (dialog.exec() != QDialog::Accepted) {
        qDebug() << "cancel";
        return 0;
    }
    qDebug() << "ok" << dialog.getViewSize();
#endif

    MainWindow w;
    w.show();
    return QApplication::exec();
}
