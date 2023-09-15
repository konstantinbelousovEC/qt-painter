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

QSize defineWindowSize() {
    auto windowSize = detail::getScreenSize();
    return detail::calcWindowRelativeSize(windowSize, 0.15);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      graphicsScene_(new QGraphicsScene{this}),
      fillColorButton_(new QToolButton{}),
      strokeColorButton_(new QToolButton{}),
      strokeWidthSpinBox_(new QSpinBox{}),
      stackedWidget_(new QStackedWidget{this}),
      modeButtonsToolBar_(new QToolBar{this}),
      windowSize_(defineWindowSize()),
      isModified_(false)
{
    addGraphicsViews();
    setUpScreen();
    setUpScene();
    setUpLayout();
    setUpToolBarStyle();
    modeButtonsToolBar_->addSeparator();
    test();
    setUpToolBarActionsConnections();
}

MainWindow::~MainWindow() = default;

void MainWindow::test() {
    QPixmap pixmap{QSize{24,24}};
    pixmap.fill(Qt::red);

    fillColorButton_->setIcon(pixmap);
    fillColorAction_ = modeButtonsToolBar_->addWidget(fillColorButton_);
    fillColorAction_->setVisible(false);

    strokeColorButton_->setIcon(pixmap);
    strokeColorAction_ = modeButtonsToolBar_->addWidget(strokeColorButton_);
    strokeColorAction_->setVisible(false);

    strokeWidthAction_ = modeButtonsToolBar_->addWidget(strokeWidthSpinBox_);
    strokeWidthSpinBox_->setMinimum(1);
    strokeWidthSpinBox_->setMaximum(30);
    strokeWidthSpinBox_->setValue(1);
    strokeWidthAction_->setVisible(false);
}

void MainWindow::setUpToolBarStyle() {
    QFile file(kToolBarStyleSheetPath.data());
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    modeButtonsToolBar_->setStyleSheet(strCSS);
    modeButtonsList_.front()->setChecked(true);
}

void MainWindow::addMode(std::string_view iconPath, int btnIndex) {
    auto* button = addToolBarButton(iconPath);
    connect(button, &QPushButton::clicked, this, [&, button, btnIndex]() {
        stackedWidget_->setCurrentIndex(btnIndex);
        for (auto* btn : modeButtonsList_) {
            btn->setChecked(false);
        }
        button->setChecked(true);
        graphicsScene_->clearSelection();

        if (btnIndex == 0) {
            fillColorAction_->setVisible(false);
            strokeColorAction_->setVisible(false);
            strokeWidthAction_->setVisible(false);
        } else {
            // todo: actions visibility processor
            changeActionsVisibility(btnIndex);
        }
    });
}

void MainWindow::setUpToolBarActionsConnections() {
    connect(fillColorButton_, &QToolButton::clicked, this, &MainWindow::setFillColor);
    connect(strokeColorButton_, &QToolButton::clicked, this, &MainWindow::setStrokeColor);
    connect(strokeWidthSpinBox_, &QSpinBox::valueChanged, this, &MainWindow::setStrokeWidth);
}

void MainWindow::changeActionsVisibility(int btnIndex) {
    if (btnIndex == 6 || btnIndex == 7) {
        setUpModePropertiesToolButtons<2>(viewsList_[btnIndex - 1]);
    } else {
        setUpModePropertiesToolButtons<3>(viewsList_[btnIndex - 1]);
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

    modeButtonsToolBar_->addWidget(button);
    return button;
}

void MainWindow::addGraphicsViews() {
    modificationModeView_ = setUpGraphicView<ModificationModeView>(kModificationModeIconPath);
    squareModeView_ = setUpGraphicView<SquareModeView>(kSquareModeIconPath);
    rectangleModeView_ = setUpGraphicView<RectangleModeView>(kRectangleModeIconPath);
    triangleModeView_ = setUpGraphicView<TriangleModeView>(kTriangleModeIconPath);
    polygonModeView_ = setUpGraphicView<PolygonModeView>(kPolygonModeIconPath);
    circleModeView_ = setUpGraphicView<CircleModeView>(kCircleModeIconPath);
    lineModeView_ = setUpGraphicView<LineModeView>(kLineModeIconPath);
    brushModeView_ = setUpGraphicView<BrushModeView>(kBrushModeIconPath);
}

void MainWindow::setUpLayout() {
    auto* verticalLayout = new QVBoxLayout{};
    verticalLayout->addWidget(modeButtonsToolBar_);
    verticalLayout->addWidget(stackedWidget_);

    auto* centralWidget = new QWidget{};
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);
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
        viewsList_[viewIndex - 1]->setFillColor(color);
        QPixmap pixmap{24, 24};
        pixmap.fill(color);
        fillColorButton_->setIcon(pixmap);
    }
}

void MainWindow::setStrokeColor() {
    QColor color = QColorDialog::getColor();
    if (color.isValid()) {
        int viewIndex = stackedWidget_->currentIndex();
        viewsList_[viewIndex - 1]->setStrokeColor(color);
        QPixmap pixmap{24, 24};
        pixmap.fill(color);
        strokeColorButton_->setIcon(pixmap);
    }
}

void MainWindow::setStrokeWidth(int width) {
    assert(width >= 0);
    int viewIndex = stackedWidget_->currentIndex();
    viewsList_[viewIndex - 1]->setStrokeWidth(width);
}
