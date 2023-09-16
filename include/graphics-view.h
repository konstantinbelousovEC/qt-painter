// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class ApplicationGraphicsView : public QGraphicsView {
    Q_OBJECT

 public:
    ApplicationGraphicsView(QGraphicsScene* scene, QSize viewSize);
    ~ApplicationGraphicsView() override = default;

 private: // todo: private or protected ?
    QSize viewSize_;
};
