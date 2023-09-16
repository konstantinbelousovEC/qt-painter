// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "drawing-graphics-view.h"

class CircleModeView : public DrawingGraphicsView {
    Q_OBJECT

 public:
    explicit CircleModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 signals:
    void changeStateOfScene();

 private:
    QGraphicsEllipseItem* currentItem_;
    QPointF ellipseCenterPos_;
};
