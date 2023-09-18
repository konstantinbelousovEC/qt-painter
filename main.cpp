// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QApplication>
#include "./include/main-window.h"
#include "./include/welcome-dialog.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    WelcomeDialog dialog;

    if (dialog.exec() != QDialog::Accepted) return 0;
    QSize viewSize{dialog.getViewSize()};

    MainWindow w{viewSize};
    w.show();

    return QApplication::exec();
}
