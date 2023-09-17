// @copyright Copyright (c) 2023 by Konstantin Belousov

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
#include <QScreen>
#include <QLabel>
#include <string_view>
#include "../include/graphics-items-detail.h"
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

    constexpr auto kChooseColorSuggestion{"Choose color"sv};

    constexpr Qt::GlobalColor kDefaultSceneBackgroundColor{Qt::white};
    constexpr QSize kDefaultBtnIconSize{24, 24};
    constexpr QSize kDefaultViewsSize{1700, 600};

    constexpr int kDefaultStatusBarCursorLabelSize{45};

}  // namespace

QSize defineWindowSize(); // todo: change the way of screen settings
QSize calcWindowRelativeSize(QSize wSize, double x);

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
      labelCursorPosX_(new QLabel{this}),
      labelCursorPosY_(new QLabel{this}),
      windowSize_(defineWindowSize()),
      graphicsViewsSize_(kDefaultViewsSize),
      isModified_(false)
{
    setUpScreen();
    setUpWidgetsPlacement();
    setUpScene();
    addGraphicsViews();
    setUpApplicationStyles();
    setUpDrawingPropertiesButtons();
    setUpToolBarActionsConnections();
    setUpStatusBar();
    setUpConnectionsForStatusBar();

    // testFeaturesMethod(); // todo: feature's testing
}

void MainWindow::testFeaturesMethod() {

}

void MainWindow::testFeatureSlot() {

}

QSize defineWindowSize() {
    auto windowSize = QGuiApplication::primaryScreen()->size();
    return calcWindowRelativeSize(windowSize, 0.15);
}

MainWindow::~MainWindow() = default;

void MainWindow::setUpScreen() {
    auto [screenWidth, screenHeight] = QGuiApplication::primaryScreen()->size();
    auto [windowWidth, windowHeight] =
            calcWindowRelativeSize(QSize{screenWidth, screenHeight}, 0.15);

    setMinimumSize(windowWidth / 2, windowHeight / 2);
    setGeometry((screenWidth - windowWidth) / 2,
                (screenHeight - windowHeight) / 2,
                windowWidth,
                windowHeight);
}

QSize calcWindowRelativeSize(QSize wSize, double x) { // todo: i don't like it
    auto [screenWidth, screenHeight] = wSize;
    int windowWidth = screenWidth - static_cast<int>(screenWidth * x);
    int windowHeight = screenHeight - static_cast<int>(screenHeight * x);
    return {windowWidth, windowHeight};
}

void MainWindow::setUpStackedWidgetLayout() {
    auto* centralWidget = new QWidget{this};
    auto* layout = new QVBoxLayout{centralWidget};
    layout->addWidget(stackedWidget_, 0, Qt::AlignCenter);
    setCentralWidget(centralWidget);
}

void MainWindow::setUpWidgetsPlacement() {
    addToolBar(toolBar_);
    setStatusBar(statusBar_);
    setUpStackedWidgetLayout();
}

void MainWindow::setUpScene() {
    graphicsScene_->setItemIndexMethod(QGraphicsScene::NoIndex);                    // setting up indexing of elements - https://doc.qt.io/qt-6/qgraphicsscene.html#itemIndexMethod-prop
    graphicsScene_->setBackgroundBrush(QBrush{kDefaultSceneBackgroundColor});
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
    toolBar_->addSeparator(); // todo: i don't like it
}

template<typename Widget>
void addStyleSheetsToWidget(Widget* widget, std::string_view qssPath);

void MainWindow::setUpApplicationStyles() {
    addStyleSheetsToWidget(toolBar_, kToolBarStyleSheetPath);
    modeButtonsList_.front()->setChecked(true);
}

template<typename Widget>
void addStyleSheetsToWidget(Widget* widget, std::string_view qssPath) {
    QFile file(qssPath.data());
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    widget->setStyleSheet(strCSS);
}

void MainWindow::setUpDrawingPropertiesButtons() {
    setUpToolBarColorButtons(fillColorButton_, fillColorAction_);
    setUpToolBarColorButtons(strokeColorButton_, strokeColorAction_);
    setUpToolBarSpinBox(strokeWidthSpinBox_, strokeWidthAction_);
}

void MainWindow::setUpToolBarColorButtons(QToolButton* button, QAction*& action) {
    button->setFixedSize(kDefaultBtnIconSize);
    button->setAutoFillBackground(true);
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

void MainWindow::setUpToolBarActionsConnections() {
    connect(fillColorButton_, &QToolButton::clicked, this, &MainWindow::setFillColor);
    connect(strokeColorButton_, &QToolButton::clicked, this, &MainWindow::setStrokeColor);
    connect(strokeWidthSpinBox_, &QSpinBox::valueChanged, this, &MainWindow::setStrokeWidth);
}

inline void addLabelToStatusBar(QStatusBar* statusBar, QLabel* label, int labelWidth) {
    label->setFixedWidth(labelWidth);
    statusBar->addWidget(label);
}

void MainWindow::setUpStatusBar() {
    addLabelToStatusBar(statusBar_, labelCursorPosX_, kDefaultStatusBarCursorLabelSize);
    addLabelToStatusBar(statusBar_, labelCursorPosY_, kDefaultStatusBarCursorLabelSize);
}

void MainWindow::setUpConnectionsForStatusBar() {
    connect(modificationModeView_, &ApplicationGraphicsView::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
    connect(modificationModeView_, &ApplicationGraphicsView::cursorHasLeavedView, this, &MainWindow::hideCursorLabelsFromStatusBar);
    for (auto* view : drawingViewsList_) {
        connect(view, &ApplicationGraphicsView::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
        connect(view, &ApplicationGraphicsView::cursorHasLeavedView, this, &MainWindow::hideCursorLabelsFromStatusBar);
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

void MainWindow::changeActionsVisibility(int btnIndex) {
    if (btnIndex == 6 || btnIndex == 7) {
        setUpModePropertiesToolButtons<2>(drawingViewsList_[btnIndex - 1]);
    } else {
        setUpModePropertiesToolButtons<3>(drawingViewsList_[btnIndex - 1]);
    }
}

void MainWindow::hideAllPropertiesActions() {
    for (auto* action : modePropertiesActions_) {
        action->setVisible(false);
    }
}

// Slots:

void MainWindow::updateCursorPosition(QPointF position) { // todo: make it serious
    labelCursorPosX_->setVisible(true);
    labelCursorPosY_->setVisible(true);
    labelCursorPosX_->setText(QString{"x: %1"}.arg(position.x()));
    labelCursorPosY_->setText(QString{"y: %1"}.arg(position.y()));
}

void MainWindow::hideCursorLabelsFromStatusBar() {
    labelCursorPosX_->setVisible(false);
    labelCursorPosY_->setVisible(false);
}

void MainWindow::setStrokeWidth(int width) {
    assert(width >= 0);
    int viewIndex = stackedWidget_->currentIndex();
    drawingViewsList_[viewIndex - 1]->setStrokeWidth(width);
}

void MainWindow::changeSceneState() {
    isModified_ = true;
}

enum class ColorButtonType {
    FILL,
    STROKE
};

template<ColorButtonType BtnType>
void setColor(const QStackedWidget* stackedWidget,
              const QList<DrawingGraphicsView*>& drawingViewsList,
              QToolButton* toolButton);

void MainWindow::setStrokeColor() {
    setColor<ColorButtonType::STROKE>(stackedWidget_,
                                      drawingViewsList_,
                                      strokeColorButton_);
}

void MainWindow::setFillColor() {
    setColor<ColorButtonType::FILL>(stackedWidget_,
                                    drawingViewsList_,
                                    fillColorButton_);
}

template<ColorButtonType BtnType>
void setColor(const QStackedWidget* stackedWidget,
              const QList<DrawingGraphicsView*>& drawingViewsList,
              QToolButton* toolButton)
{
    QColor color = QColorDialog::getColor(Qt::white,
                                          nullptr,
                                          kChooseColorSuggestion.data(),
                                          QColorDialog::ShowAlphaChannel);
    if (color.isValid()) {
        int viewIndex = stackedWidget->currentIndex();

        if constexpr (BtnType == ColorButtonType::STROKE)
            drawingViewsList[viewIndex - 1]->setStrokeColor(color);
        else if constexpr (BtnType == ColorButtonType::FILL)
            drawingViewsList[viewIndex - 1]->setFillColor(color);

        toolButton->setPalette(QPalette{color});
    }
}

void showPropertiesButtons(QToolButton* button, QAction* action, const QColor& color) {
    button->setAutoFillBackground(true);
    button->setPalette(QPalette{color});
    action->setVisible(true);
}

void showPropertiesButtons(QSpinBox* spinBox, QAction* action, int width) {
    spinBox->setValue(width);
    action->setVisible(true);
}
