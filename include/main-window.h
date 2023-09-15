// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QToolButton>
#include <QSpinBox>
#include <QMenu>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsView;
class QPushButton;
class QSpinBox;
class QToolButton;
class ModificationModeView;
class DrawingGraphicsView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

 private:
    void setUpModePropertiesButtonsAndActions(); // todo: refactor
    void setUpToolBarActionsConnections();
    void changeActionsVisibility(int btnIndex);
    void addGraphicsViews();
    void setUpLayout();
    void setUpScene();
    void setUpScreen();
    void setUpToolBarStyle();
    QPushButton* addToolBarButton(std::string_view iconPath);
    void addModeButtonsAndConnections(std::string_view iconPath, int btnIndex);

    template<typename GraphicViewType, typename Signal>
    void connectViewsSignals(GraphicViewType view, Signal signal);

    template<typename GraphicViewType>
    void setUpGraphicView(std::string_view iconPath);

    template<int PropertiesAmount, typename ModeView>
    void setUpModePropertiesToolButtons(ModeView view);

 private slots:
    void changeSceneState();
    void setFillColor();
    void setStrokeColor();
    void hideAllPropertiesActions();
    void setStrokeWidth(int width);
    void setUpToolBarColorButtons(QToolButton* button, QAction*& action);
    void setUpToolBarSpinBox(QSpinBox* spinBox, QAction*& action);

 private:
    QList<DrawingGraphicsView*> drawingViewsList_;
    QList<QPushButton*> modeButtonsList_;
    QList<QAction*> modePropertiesActions_;

    QGraphicsScene* graphicsScene_;
    QStackedWidget* stackedWidget_;
    QToolBar* modeButtonsToolBar_;

    QToolButton* fillColorButton_;
    QToolButton* strokeColorButton_;
    QSpinBox* strokeWidthSpinBox_;

    QAction* fillColorAction_{};
    QAction* strokeColorAction_{};
    QAction* strokeWidthAction_{};

    QSize windowSize_;
    bool isModified_;
};

template<typename GraphicViewType, typename Signal>
void MainWindow::connectViewsSignals(GraphicViewType view, Signal signal) {
    connect(view, signal, this, &MainWindow::changeSceneState);
}

template<typename GraphicViewType>
void MainWindow::setUpGraphicView(std::string_view iconPath) {
    auto* view = new GraphicViewType{graphicsScene_};
    auto viewIndex = stackedWidget_->addWidget(view);
    addModeButtonsAndConnections(iconPath, viewIndex);
    connectViewsSignals(view, &GraphicViewType::changeStateOfScene);
    if constexpr (!std::is_same_v<GraphicViewType, ModificationModeView>) drawingViewsList_.push_back(view);
}

void showPropertiesButtons(QToolButton* button, QAction* action, const QColor& color);
void showPropertiesButtons(QSpinBox* spinBox, QAction* action, int width);

template<int PropertiesAmount, typename ModeView>
void MainWindow::setUpModePropertiesToolButtons(ModeView view) {
    if constexpr (PropertiesAmount == 3) {
        showPropertiesButtons(fillColorButton_, fillColorAction_, view->getFillColor());
    } else if constexpr (PropertiesAmount == 2) {
        fillColorAction_->setVisible(false);
    }

    showPropertiesButtons(strokeColorButton_, strokeColorAction_, view->getStrokeColor());
    showPropertiesButtons(strokeWidthSpinBox_, strokeWidthAction_, view->getStrokeWidth());
}
