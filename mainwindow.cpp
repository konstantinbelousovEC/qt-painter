#include "mainwindow.h"
#include "rectanglemodeview.h"
#include "squaremodeview.h"
#include "circlemodeview.h"
#include "trianglemodeview.h"
#include "modificationmodevew.h"

#include <QMainWindow>
#include <QGraphicsView>
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QStringView>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

namespace {

    using namespace std::string_view_literals;

    constexpr std::string_view MODIFING_MODE_ICON_PATH{":/images/buttons/imgs/modimg.png"sv};
    constexpr std::string_view SQUARE_MODE_ICON_PATH{":/images/buttons/imgs/squareimg.png"};
    constexpr std::string_view RECT_MODE_ICON_PATH{":/images/buttons/imgs/rectimg.png"};
    constexpr std::string_view TRIANGLE_MODE_ICON_PATH{":/images/buttons/imgs/trianimg.png"};
    constexpr std::string_view CIRCLE_MODE_ICON_PATH{":/images/buttons/imgs/circleimg.png"};

    constexpr QSize DEFAULT_BUTTON_ICON_SIZE{30,30};
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      scene_(new QGraphicsScene(this)),
      stackedWidget_(new QStackedWidget(this)),
      toolBar_(new QToolBar(this))
{
    setMinimumSize(800, 600);
    setUpScene();
    setUpGraphicViews();
    setUpToolbar();
    setUpLayout();
    setUpMenuBar();
}

MainWindow::~MainWindow() {
    delete scene_;
    delete stackedWidget_;
    delete toolBar_;
}

void MainWindow::setUpToolbar() {
    QPushButton *button1 = new QPushButton();
    QPushButton *button2 = new QPushButton();
    QPushButton *button3 = new QPushButton();
    QPushButton *button4 = new QPushButton();
    QPushButton *button5 = new QPushButton();

    QPixmap pixmap1(MODIFING_MODE_ICON_PATH.data());
    button1->setIcon(QIcon(pixmap1));
    button1->setIconSize(DEFAULT_BUTTON_ICON_SIZE);

    QPixmap pixmap2(SQUARE_MODE_ICON_PATH.data());
    button2->setIcon(QIcon(pixmap2));
    button2->setIconSize(DEFAULT_BUTTON_ICON_SIZE);

    QPixmap pixmap3(RECT_MODE_ICON_PATH.data());
    button3->setIcon(QIcon(pixmap3));
    button3->setIconSize(DEFAULT_BUTTON_ICON_SIZE);

    QPixmap pixmap4(TRIANGLE_MODE_ICON_PATH.data());
    button4->setIcon(QIcon(pixmap4));
    button4->setIconSize(DEFAULT_BUTTON_ICON_SIZE);

    QPixmap pixmap5(CIRCLE_MODE_ICON_PATH.data());
    button5->setIcon(QIcon(pixmap5));
    button5->setIconSize(DEFAULT_BUTTON_ICON_SIZE);

    toolBar_->addWidget(button1);
    toolBar_->addWidget(button2);
    toolBar_->addWidget(button3);
    toolBar_->addWidget(button4);
    toolBar_->addWidget(button5);

    connect(button1, &QPushButton::clicked, this, [=]() {
        stackedWidget_->setCurrentIndex(0);
    });
    connect(button2, &QPushButton::clicked, this, [=]() {
        stackedWidget_->setCurrentIndex(1);
    });
    connect(button3, &QPushButton::clicked, this, [=]() {
        stackedWidget_->setCurrentIndex(2);
    });
    connect(button4, &QPushButton::clicked, this, [=]() {
        stackedWidget_->setCurrentIndex(3);
    });
    connect(button5, &QPushButton::clicked, this, [=]() {
        stackedWidget_->setCurrentIndex(4);
    });
}

void MainWindow::setUpGraphicViews() {
    stackedWidget_->addWidget(new ModificationModeVew(scene_));
    stackedWidget_->addWidget(new SquareModeView(scene_));
    stackedWidget_->addWidget(new RectangleModeView(scene_));
    stackedWidget_->addWidget(new TriangleModeView(scene_));
    stackedWidget_->addWidget(new CircleModeView(scene_));
}

void MainWindow::setUpLayout() {
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(toolBar_);
    layout->addWidget(stackedWidget_);

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::setUpScene() {
    scene_->setBackgroundBrush(QBrush{Qt::white});
    scene_->setStickyFocus(true);
    scene_->setSceneRect(0, 0, 750, 450);
}

void MainWindow::setUpMenuBar() {
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));

    QAction *newAction = new QAction(tr("New"), this);
    QAction *loadAction = new QAction(tr("Load"), this);
    QAction *saveAction = new QAction(tr("Save"), this);
    QAction *saveAsAction = new QAction(tr("Save as..."), this);
    QAction *someAction = new QAction(tr("Leave"), this);

    fileMenu->addAction(newAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(someAction);

    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(someAction, &QAction::triggered, this, &MainWindow::someApp);
}

void MainWindow::newFile() {

}

void MainWindow::loadFile() {

}

void MainWindow::saveFile() {

}

void MainWindow::saveFileAs() {
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Save Image",
        QDir::homePath(),
        "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp);;All Files (*)"
    );

    if (!filePath.isEmpty()) {
        QPixmap pixmap(scene_->sceneRect().size().toSize());
        QPainter painter(&pixmap);
        scene_->render(&painter);
        pixmap.save(filePath);
    }
}

void MainWindow::someApp() {
    QCoreApplication::exit();
}
