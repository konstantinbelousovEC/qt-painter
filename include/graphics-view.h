// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class ApplicationGraphicsView : public QGraphicsView {
    Q_OBJECT

 public:
    ApplicationGraphicsView(QGraphicsScene* scene, int viewWidth, int viewHeight);
    ~ApplicationGraphicsView() override = default;

 private: // todo: private or protected ?
    int viewWidth_;
    int viewHeight_;
};
