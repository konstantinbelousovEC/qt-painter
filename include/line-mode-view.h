// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "drawing-graphics-view.h"

class LineModeView : public DrawingGraphicsView {
 public:
    explicit LineModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 private:
    QGraphicsLineItem* currentItem_;
    QPointF startCursorPos_;
};