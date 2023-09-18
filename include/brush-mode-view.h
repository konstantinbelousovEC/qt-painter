// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "drawing-graphics-view.h"

class BrushModeView : public DrawingGraphicsView {
 public:
    BrushModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 private:
    QList<QGraphicsLineItem*> temporaryLines_;
    QGraphicsEllipseItem* startEllipseItem_;
    QPointF startCursorPos_;
    QPointF previousCursorPos_;
};
