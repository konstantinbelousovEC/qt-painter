#include "mainwindow.h"
#include "rectanglemodeview.h"
#include "modificationmodeview.h"
#include "circlemodeview.h"
#include "squaremodeview.h"

#include <QCoreApplication>
#include <QGraphicsView>
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStringView>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>

namespace {

    using namespace std::string_view_literals;

    constexpr std::string_view MODIFIING_MODE_ICON_PATH{":/images/buttons/imgs/modimg.png"sv};
    constexpr std::string_view SQUARE_MODE_ICON_PATH{":/images/buttons/imgs/squareimg.png"};
    constexpr std::string_view RECT_MODE_ICON_PATH{":/images/buttons/imgs/rectimg.png"};
    constexpr std::string_view TRIANGLE_MODE_ICON_PATH{":/images/buttons/imgs/trianimg.png"};
    constexpr std::string_view CIRCLE_MODE_ICON_PATH{":/images/buttons/imgs/circleimg.png"};

    constexpr QSize kDefaultBtnIconSize{30,30};
    
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      scene_(new QGraphicsScene{this}),
      stackedWidget_(new QStackedWidget{this}),
      toolBar_(new QToolBar{this}),
      isModified_(false)
{
    setMinimumSize(1000, 700);
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
    auto* button1 = new QPushButton{};
    auto* button2 = new QPushButton{};
    auto* button3 = new QPushButton{};
    auto* button4 = new QPushButton{};

    QPixmap pixmap1{MODIFIING_MODE_ICON_PATH.data()};
    button1->setIcon(QIcon{pixmap1});
    button1->setIconSize(kDefaultBtnIconSize);

    QPixmap pixmap2{RECT_MODE_ICON_PATH.data()};
    button2->setIcon(QIcon{pixmap2});
    button2->setIconSize(kDefaultBtnIconSize);

    QPixmap pixmap3{CIRCLE_MODE_ICON_PATH.data()};
    button3->setIcon(QIcon{pixmap3});
    button3->setIconSize(kDefaultBtnIconSize);

    QPixmap pixmap4{SQUARE_MODE_ICON_PATH.data()};
    button4->setIcon(QIcon{pixmap4});
    button4->setIconSize(kDefaultBtnIconSize);

    toolBar_->addWidget(button1);
    toolBar_->addWidget(button2);
    toolBar_->addWidget(button3);
    toolBar_->addWidget(button4);

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
}

void MainWindow::setUpGraphicViews() {
    stackedWidget_->addWidget(new ModificationModeView{scene_});
    stackedWidget_->addWidget(new RectangleModeView{scene_});
    stackedWidget_->addWidget(new CircleModeView{scene_});
    stackedWidget_->addWidget(new SquareModeView{scene_});
}

void MainWindow::setUpLayout() {
    auto* layout = new QVBoxLayout{};
    layout->addWidget(toolBar_);
    layout->addWidget(stackedWidget_);

    auto* centralWidget = new QWidget{};
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::setUpScene() {
    scene_->setBackgroundBrush(QBrush{Qt::white});
    scene_->setStickyFocus(false);
    // scene_->setSceneRect(0, 0, 900, 600);
    auto* item = scene_->addRect(QRectF{QPointF{0,0}, QSizeF(1, 1)},
                                                         QPen{Qt::black},
                                                         QBrush{Qt::black}); // todo: center for tests
}

void MainWindow::setUpMenuBar() {
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));

    auto* newAction = new QAction{tr("New"), this};
    auto* loadAction = new QAction{tr("Load"), this};
    auto* saveAction = new QAction{tr("Save"), this};
    auto* saveAsAction = new QAction{tr("Save as..."), this};
    auto* someAction = new QAction{tr("Leave"), this};

    fileMenu->addAction(newAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(someAction);

    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(someAction, &QAction::triggered, this, &MainWindow::exitApp);
}

void MainWindow::newFile() {
    if (isModified_) {
        int ret = QMessageBox::warning(this, "Save Changes",
                                       "Do you want to save your changes?",
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            saveFile();
        } else if (ret == QMessageBox::Cancel) {
            return;
        }
    }

    scene_->clear();
    isModified_ = false;
}

void MainWindow::loadFile() {
    if (isModified_) {
        int ret = QMessageBox::warning(this, "Save Changes",
                                       "Do you want to save your changes?",
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            saveFile();
        } else if (ret == QMessageBox::Cancel) {
            return;
        }
    }

    QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Images (*.png *.jpg)");
    if (!filePath.isEmpty()) {
        QImage image(filePath);
        if (!image.isNull()) {
            scene_->clear();
            scene_->addPixmap(QPixmap::fromImage(image));
            isModified_ = false;
        }
    }
}

void MainWindow::saveFile() {
    if (currentFilePath_.isEmpty()) {
        saveFileAs();
    } else {
        QFile file(currentFilePath_);
        if (file.open(QIODevice::WriteOnly)) {
            foreach(const auto item, scene_->items()) {
                //item->
            }
        }
        scene_->clearSelection();
        QImage image(scene_->sceneRect().size().toSize(), QImage::Format_ARGB32);
        image.fill(Qt::white);
        QPainter painter(&image);
        scene_->render(&painter);
        image.save(currentFilePath_);
        isModified_ = false;
    }
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
        isModified_ = false;
    }
}

void MainWindow::exitApp() {
    if (isModified_) {
        int ret = QMessageBox::warning(this, "Save Changes and Exit",
                                       "Do you want to save your changes before exiting?",
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            saveFile();
        } else if (ret == QMessageBox::Cancel) {
            return;
        }
    }
    QCoreApplication::exit();
}
