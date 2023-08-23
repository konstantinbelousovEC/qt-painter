#pragma once

#include <QMainWindow>
#include <QGraphicsScene>
#include <QStackedWidget>

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
    void setUpToolbar();
    void setUpGraphicViews();
    void setUpLayout();
    void setUpScene();
    void setUpMenuBar();

private slots:
    void newFile();
    void loadFile();
    void saveFile();
    void saveFileAs();
    void exitApp();

};
