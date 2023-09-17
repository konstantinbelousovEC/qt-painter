// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "drawing-graphics-view.h"

class SquareModeView : public DrawingGraphicsView {
 public:
    explicit SquareModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 private:
    QGraphicsRectItem* currentItem_;
    QPointF centerPos_;
};
