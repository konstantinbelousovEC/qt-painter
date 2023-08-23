// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QMainWindow>
#include <QGraphicsScene>
#include <QStackedWidget>
#include <QMenu>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QGraphicsScene* scene_;
    QStackedWidget* stackedWidget_;
    QToolBar* toolBar_;
    bool isModified_;
    QString currentFilePath_;

private:
    void setUpGraphicViews();
    void setUpLayout();
    void setUpScene();
    void setUpMenuBar();
    void addMode(std::string_view iconPath, int btnIndex);

    template<typename Func>
    void addMenuAction(QMenu* menu, std::string_view actionName, Func func);

private slots:
    void newFile();
    void loadFile();
    void saveFile();
    void saveFileAs();
    void exitApp();

};

template<typename Func>
void MainWindow::addMenuAction(QMenu* menu, std::string_view actionName, Func func) {
    auto* newAction = new QAction{actionName.data(), this};
    menu->addAction(newAction);
    connect(newAction, &QAction::triggered, this, func);
}
