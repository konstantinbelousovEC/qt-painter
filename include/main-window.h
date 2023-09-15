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
class SquareModeView;
class RectangleModeView;
class TriangleModeView;
class PolygonModeView;
class CircleModeView;
class LineModeView;
class BrushModeView;
class DrawingGraphicsView;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

 private:
    void test(); // todo: refactor
    void setUpToolBarActionsConnections();
    void changeActionsVisibility(int btnIndex);

    void addGraphicsViews();
    void setUpLayout();
    void setUpScene();
    void setUpScreen();
    void setUpToolBarStyle();
    QPushButton* addToolBarButton(std::string_view iconPath);
    void addMode(std::string_view iconPath, int btnIndex);

    template<typename GraphicViewType, typename Signal>
    void connectViewsSignals(GraphicViewType view, Signal signal);

    template<typename GraphicViewType>
    GraphicViewType* setUpGraphicView(std::string_view iconPath);

    template<int PropertiesAmount, typename ModeView>
    void setUpModePropertiesToolButtons(ModeView view);

 private slots:
    void changeSceneState();
    void setFillColor();
    void setStrokeColor();
    void setStrokeWidth(int width);

 private:
    QList<QPushButton*> modeButtonsList_;
    QList<DrawingGraphicsView*> drawingViewsList_;
    QGraphicsScene* graphicsScene_;

    ModificationModeView*   modificationModeView_;
    SquareModeView*         squareModeView_;
    RectangleModeView*      rectangleModeView_;
    TriangleModeView*       triangleModeView_;
    PolygonModeView*        polygonModeView_;
    CircleModeView*         circleModeView_;
    LineModeView*           lineModeView_;
    BrushModeView*          brushModeView_;

    QToolButton* fillColorButton_;
    QToolButton* strokeColorButton_;
    QSpinBox* strokeWidthSpinBox_;

    QAction* fillColorAction_;
    QAction* strokeColorAction_;
    QAction* strokeWidthAction_;

    QStackedWidget* stackedWidget_;
    QToolBar* modeButtonsToolBar_;
    QSize windowSize_;
    bool isModified_;

};

template<typename GraphicViewType, typename Signal>
void MainWindow::connectViewsSignals(GraphicViewType view, Signal signal) {
    connect(view, signal, this, &MainWindow::changeSceneState);
}

template<typename GraphicViewType>
GraphicViewType* MainWindow::setUpGraphicView(std::string_view iconPath) {
    auto* view = new GraphicViewType{graphicsScene_};
    auto viewIndex = stackedWidget_->addWidget(view);
    addMode(iconPath, viewIndex);
    connectViewsSignals(view, &GraphicViewType::changeStateOfScene);
    if constexpr (!std::is_same_v<GraphicViewType, ModificationModeView>) drawingViewsList_.push_back(view);
    return view;
}

template<int PropertiesAmount, typename ModeView>
void MainWindow::setUpModePropertiesToolButtons(ModeView view) {
    QPixmap pixmap{24, 24};
    if constexpr (PropertiesAmount == 3) {
        pixmap.fill(view->getFillColor());
        fillColorButton_->setIcon(pixmap);
        fillColorAction_->setVisible(true);
    } else if constexpr (PropertiesAmount == 2) {
        fillColorAction_->setVisible(false);
    }

    pixmap.fill(view->getStrokeColor());
    strokeColorButton_->setIcon(pixmap);
    strokeColorAction_->setVisible(true);

    strokeWidthSpinBox_->setValue(view->getStrokeWidth());
    strokeWidthAction_->setVisible(true);
}
