// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenu>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QPushButton;
QT_END_NAMESPACE

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
    void setUpToolBarStyle();
    QPushButton* addToolBarButton(std::string_view iconPath);
    void addMode(std::string_view iconPath, int btnIndex);

    template<typename GraphicViewType, typename Signal>
    void connectViewsSignals(GraphicViewType view, Signal signal);

    template<typename GraphicViewType>
    void setUpGraphicView(std::string_view iconPath);

 private slots:
    void changeSceneState();

 private:
    QList<QPushButton*> modeButtonsList_;
    QGraphicsScene* graphicsScene_;
    QStackedWidget* stackedWidget_;
    QToolBar* modeButtonsToolBar_;
    QString currentFilePath_;
    QSize windowSize_;
    bool isModified_;
};

template<typename GraphicViewType, typename Signal>
void MainWindow::connectViewsSignals(GraphicViewType view, Signal signal) {
    connect(view, signal, this, &MainWindow::changeSceneState);
}

template<typename GraphicViewType>
void MainWindow::setUpGraphicView(std::string_view iconPath) {
    auto view = new GraphicViewType{graphicsScene_};
    auto viewIndex = stackedWidget_->addWidget(view);
    addMode(iconPath, viewIndex);
    connectViewsSignals(view, &GraphicViewType::changeStateOfScene);
}
