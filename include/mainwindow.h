// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QMainWindow>
#include <QGraphicsScene>
#include <QStackedWidget>
#include <QPushButton>
#include <QMenu>

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

 private:
    void setUpGraphicViews();
    void setUpLayout();
    void setUpScene();
    void setUpScreen();
    void setUpMenuBar();
    void setUpToolBarStyle();
    QPushButton* addToolBarButton(std::string_view iconPath);
    void addMode(std::string_view iconPath, int btnIndex);

    template<typename Slot>
    void addMenuAction(QMenu* menu, std::string_view actionName, Slot slot);

    template<typename GraphicViewType, typename Signal>
    void connectViewsSignals(GraphicViewType view, Signal signal);

    template<typename GraphicViewType>
    void setUpGraphicView(std::string_view iconPath);

 private slots:
    void changeSceneState();
    void newFile();
    void loadFile();
    void saveFile();
    void saveFileAs();
    void exitApp();

 private:
    QVector<QPushButton*> btnList_;
    QGraphicsScene* scene_;
    QStackedWidget* stackedWidget_;
    QToolBar* toolBar_;
    QString currentFilePath_;
    bool isModified_;
};

template<typename Slot>
void MainWindow::addMenuAction(QMenu* menu, std::string_view actionName, Slot slot) {
    auto* newAction = new QAction{actionName.data(), this};
    menu->addAction(newAction);
    connect(newAction, &QAction::triggered, this, slot);
}

template<typename GraphicViewType, typename Signal>
void MainWindow::connectViewsSignals(GraphicViewType view, Signal signal) {
    connect(view, signal, this, &MainWindow::changeSceneState);
}

template<typename GraphicViewType>
void MainWindow::setUpGraphicView(std::string_view iconPath) {
    auto modificationScene = new GraphicViewType{scene_};
    auto modificationSceneIndex = stackedWidget_->addWidget(modificationScene);
    addMode(iconPath, modificationSceneIndex);
    connectViewsSignals(modificationScene, &GraphicViewType::changeStateOfScene);
}
