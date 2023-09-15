// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QCoreApplication>
#include <QGuiApplication>
#include <QGraphicsView>
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStringView>
#include <QMenuBar>
#include <QFileDialog>
#include <QLatin1String>
#include <QToolButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QColorDialog>
#include <string_view>
#include "../include/detail.h"
#include "../include/main-window.h"
#include "../include/rectangle-mode-view.h"
#include "../include/modification-mode-view.h"
#include "../include/square-mode-view.h"
#include "../include/triangle-mode.h"
#include "../include/circle-mode-view.h"
#include "../include/brush-mode-view.h"
#include "../include/line-mode-view.h"
#include "../include/polygon-mode-view.h"


namespace {
    using std::operator ""sv;

    constexpr auto kModificationModeIconPath{":/images/buttons/imgs/modimg.png"sv};
    constexpr auto kSquareModeIconPath{":/images/buttons/imgs/squareimg.png"sv};
    constexpr auto kRectangleModeIconPath{":/images/buttons/imgs/rectimg.png"sv};
    constexpr auto kTriangleModeIconPath{":/images/buttons/imgs/trianimg.png"sv};
    constexpr auto kCircleModeIconPath{":/images/buttons/imgs/circleimg.png"sv};
    constexpr auto kBrushModeIconPath{":/images/buttons/imgs/brushimg.png"sv};
    constexpr auto kLineModeIconPath{":/images/buttons/imgs/lineimg.png"sv};
    constexpr auto kPolygonModeIconPath{":/images/buttons/imgs/polygonimg.png"sv};

    constexpr auto kToolBarStyleSheetPath{":/styles/toolbarbtnstylesheet.qss"sv};

    constexpr Qt::GlobalColor kDefaultSceneBackgroundColor{Qt::white};
    constexpr QSize kDefaultBtnIconSize{24, 24};

}  // namespace

inline QSize defineWindowSize() {
    auto windowSize = detail::getScreenSize();
    return detail::calcWindowRelativeSize(windowSize, 0.15);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      graphicsScene_(new QGraphicsScene{this}),
      stackedWidget_(new QStackedWidget{this}),
      toolBar_(new QToolBar{this}),
      statusBar_(new QStatusBar{this}),
      modificationModeView_(nullptr),
      fillColorButton_(new QToolButton{}),
      strokeColorButton_(new QToolButton{}),
      strokeWidthSpinBox_(new QSpinBox{}),
      fillColorAction_(nullptr),
      strokeColorAction_(nullptr),
      strokeWidthAction_(nullptr),
      windowSize_(defineWindowSize()),
      isModified_(false)
{
    setUpScreen();
    setUpScene();
    addGraphicsViews();
    setUpLayout();
    setUpToolBarStyle();
    setUpModePropertiesButtonsAndActions();
    setUpToolBarActionsConnections();
    setStatusBar(statusBar_);
    setUpConnectionsForStatusBar();
}

void MainWindow::setUpConnectionsForStatusBar() {
    connect(modificationModeView_, &ModificationModeView::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
    for (auto* view : drawingViewsList_) {
        connect(view, &DrawingGraphicsView::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::setUpToolBarColorButtons(QToolButton* button, QAction*& action) {
    QPixmap pixmap{QSize{kDefaultBtnIconSize}};
    button->setIcon(pixmap);
    action = toolBar_->addWidget(button);
    action->setVisible(false);
    modePropertiesActions_.push_back(action);
}

void MainWindow::setUpToolBarSpinBox(QSpinBox* spinBox, QAction*& action) {
    action = toolBar_->addWidget(spinBox);
    spinBox->setMinimum(0);
    spinBox->setMaximum(30);
    spinBox->setValue(1);
    action->setVisible(false);
    modePropertiesActions_.push_back(action);
}

void MainWindow::setUpModePropertiesButtonsAndActions() {
    setUpToolBarColorButtons(fillColorButton_, fillColorAction_);
    setUpToolBarColorButtons(strokeColorButton_, strokeColorAction_);
    setUpToolBarSpinBox(strokeWidthSpinBox_, strokeWidthAction_);
}

void MainWindow::setUpToolBarStyle() {
    QFile file(kToolBarStyleSheetPath.data());
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    toolBar_->setStyleSheet(strCSS);
    modeButtonsList_.front()->setChecked(true);
}

void MainWindow::hideAllPropertiesActions() {
    for (auto* action : modePropertiesActions_) {
        action->setVisible(false);
    }
}

void MainWindow::addModeButtonsAndConnections(std::string_view iconPath, int btnIndex) {
    auto* button = addToolBarButton(iconPath);
    connect(button, &QPushButton::clicked, this, [&, button, btnIndex]() {
        stackedWidget_->setCurrentIndex(btnIndex);
        for (auto* btn : modeButtonsList_) {
            btn->setChecked(false);
        }
        button->setChecked(true);
        graphicsScene_->clearSelection();

        if (btnIndex == 0) hideAllPropertiesActions();
        else changeActionsVisibility(btnIndex);
    });
}

void MainWindow::setUpToolBarActionsConnections() {
    connect(fillColorButton_, &QToolButton::clicked, this, &MainWindow::setFillColor);
    connect(strokeColorButton_, &QToolButton::clicked, this, &MainWindow::setStrokeColor);
    connect(strokeWidthSpinBox_, &QSpinBox::valueChanged, this, &MainWindow::setStrokeWidth);
}

void MainWindow::changeActionsVisibility(int btnIndex) {
    if (btnIndex == 6 || btnIndex == 7) {
        setUpModePropertiesToolButtons<2>(drawingViewsList_[btnIndex - 1]);
    } else {
        setUpModePropertiesToolButtons<3>(drawingViewsList_[btnIndex - 1]);
    }
}

QPushButton* MainWindow::addToolBarButton(std::string_view iconPath) {
    auto* button = new QPushButton{};
    modeButtonsList_.push_back(button);
    QPixmap pixmap{iconPath.data()};

    button->setIcon(QIcon{pixmap});
    button->setIconSize(kDefaultBtnIconSize);
    button->setCheckable(true);
    button->setChecked(false);

    toolBar_->addWidget(button);
    return button;
}

void MainWindow::addGraphicsViews() {
    setUpGraphicView<ModificationModeView>(kModificationModeIconPath);
    setUpGraphicView<SquareModeView>(kSquareModeIconPath);
    setUpGraphicView<RectangleModeView>(kRectangleModeIconPath);
    setUpGraphicView<TriangleModeView>(kTriangleModeIconPath);
    setUpGraphicView<PolygonModeView>(kPolygonModeIconPath);
    setUpGraphicView<CircleModeView>(kCircleModeIconPath);
    setUpGraphicView<LineModeView>(kLineModeIconPath);
    setUpGraphicView<BrushModeView>(kBrushModeIconPath);
    toolBar_->addSeparator();
}

void MainWindow::setUpLayout() {
    addToolBar(toolBar_);
    setCentralWidget(stackedWidget_);
}

void MainWindow::setUpScene() {
    graphicsScene_->setItemIndexMethod(QGraphicsScene::NoIndex);                    // setting up indexing of elements - https://doc.qt.io/qt-6/qgraphicsscene.html#itemIndexMethod-prop
    graphicsScene_->setBackgroundBrush(QBrush{kDefaultSceneBackgroundColor});
    QSize sceneSize = detail::calcWindowRelativeSize(windowSize_, 0.1);
    graphicsScene_->setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
}

void MainWindow::setUpScreen() {
    auto [screenWidth, screenHeight] = detail::getScreenSize();
    auto [windowWidth, windowHeight] =
            detail::calcWindowRelativeSize(QSize{screenWidth, screenHeight}, 0.15);

    setMinimumSize(windowWidth, windowHeight);
    setGeometry((screenWidth - windowWidth) / 2,
                (screenHeight - windowHeight) / 2,
                windowWidth,
                windowHeight);
}

void MainWindow::changeSceneState() {
    isModified_ = true;
}

void MainWindow::setFillColor() {
    QColor color = QColorDialog::getColor();
    if (color.isValid()) {
        int viewIndex = stackedWidget_->currentIndex();
        drawingViewsList_[viewIndex - 1]->setFillColor(color);
        QPixmap pixmap{kDefaultBtnIconSize};
        pixmap.fill(color);
        fillColorButton_->setIcon(pixmap);
    }
}

void MainWindow::setStrokeColor() {
    QColor color = QColorDialog::getColor();
    if (color.isValid()) {
        int viewIndex = stackedWidget_->currentIndex();
        drawingViewsList_[viewIndex - 1]->setStrokeColor(color);
        QPixmap pixmap{kDefaultBtnIconSize};
        pixmap.fill(color);
        strokeColorButton_->setIcon(pixmap);
    }
}

void MainWindow::setStrokeWidth(int width) {
    assert(width >= 0);
    int viewIndex = stackedWidget_->currentIndex();
    drawingViewsList_[viewIndex - 1]->setStrokeWidth(width);
}

void MainWindow::updateCursorPosition(QPointF position) {
    statusBar()->showMessage(QString("x: %1 | y: %2").arg(position.x()).arg(position.y()));
}

void showPropertiesButtons(QToolButton* button, QAction* action, const QColor& color) {
    QPixmap pixmap{kDefaultBtnIconSize};
    pixmap.fill(color);
    button->setIcon(pixmap);
    action->setVisible(true);
}

void showPropertiesButtons(QSpinBox* spinBox, QAction* action, int width) {
    spinBox->setValue(width);
    action->setVisible(true);
}