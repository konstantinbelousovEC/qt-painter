// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QApplication>
#include "./include/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
