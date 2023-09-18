// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QMainWindow>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsView;
class QPushButton;
class QSpinBox;
class QLabel;
class ModificationModeView;
class DrawingGraphicsView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    explicit MainWindow(QSize viewSize, QWidget* parent = nullptr);
    ~MainWindow() override;

 private:
    QPushButton* addToolBarButton(std::string_view iconPath);
    void addModeButtonsAndConnections(std::string_view iconPath, int btnIndex);
    void setUpToolBarColorButtons(QAction*& action);
    void setUpToolBarSpinBox(QSpinBox* spinBox, QAction*& action);
    void changeActionsVisibility(int btnIndex);
    void setUpToolBarActionsConnections();
    void setUpDrawingPropertiesButtons();
    void setUpConnectionsForStatusBar();
    void hideAllPropertiesActions();
    void setUpStackedWidgetLayout();
    void setUpApplicationStyles();
    void setUpWidgetsPlacement();
    void addGraphicsViews();
    void setUpStatusBar();
    void setUpScreen();
    void setUpScene();

    template<typename GraphicsViewType, typename Signal>
    void connectViewsSignals(GraphicsViewType view, Signal signal);

    template<typename GraphicsViewType>
    void setUpGraphicView(std::string_view iconPath);

    template<int PropertiesAmount, typename ModeView>
    void setUpModePropertiesToolButtons(ModeView view);

 private slots:
    void updateCursorPosition(QPointF position);
    void hideCursorLabelsFromStatusBar();
    void setStrokeWidth(int width);
    void changeSceneState();
    void setStrokeColor();
    void setFillColor();

 private:
    QList<DrawingGraphicsView*> drawingViewsList_;
    QList<QPushButton*> modeButtonsList_;
    QList<QAction*> modePropertiesActions_;

    QGraphicsScene* graphicsScene_;
    QStackedWidget* stackedWidget_;
    QToolBar* toolBar_;
    QStatusBar* statusBar_;

    ModificationModeView* modificationModeView_;

    QSpinBox* strokeWidthSpinBox_;
    QAction* fillColorAction_{};
    QAction* strokeColorAction_{};
    QAction* strokeWidthAction_{};

    QLabel* labelCursorPosX_;
    QLabel* labelCursorPosY_;

    QSize windowSize_;
    QSize graphicsViewsSize_;
    bool isModified_;
};

template<typename GraphicsViewType, typename Signal>
void MainWindow::connectViewsSignals(GraphicsViewType view, Signal signal) {
    connect(view, signal, this, &MainWindow::changeSceneState);
}

template<typename GraphicsViewType>
void MainWindow::setUpGraphicView(std::string_view iconPath) {
    auto* view = new GraphicsViewType{graphicsScene_, graphicsViewsSize_};
    auto viewIndex = stackedWidget_->addWidget(view);
    addModeButtonsAndConnections(iconPath, viewIndex);
    connectViewsSignals(view, &GraphicsViewType::changeStateOfScene);
    if constexpr (!std::is_same_v<GraphicsViewType, ModificationModeView>) drawingViewsList_.push_back(view);
    else modificationModeView_ = view;
}

void showPropertiesButtons(QAction* action, const QColor& color);
void showPropertiesButtons(QSpinBox* spinBox, QAction* action, int width);

template<int PropertiesAmount, typename ModeView>
void MainWindow::setUpModePropertiesToolButtons(ModeView view) {
    if constexpr (PropertiesAmount == 3) {
        showPropertiesButtons(fillColorAction_, view->getFillColor());
    } else if constexpr (PropertiesAmount == 2) {
        fillColorAction_->setVisible(false);
    }

    showPropertiesButtons(strokeColorAction_, view->getStrokeColor());
    showPropertiesButtons(strokeWidthSpinBox_, strokeWidthAction_, view->getStrokeWidth());
}
