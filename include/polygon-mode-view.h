// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "drawing-graphics-view.h"

class PolygonModeView : public DrawingGraphicsView {
 public:
    explicit PolygonModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

 private:
    void createPolygon();
    void deleteTemporaryLines();

    QList<QPointF> points_;
    QList<QGraphicsLineItem*> lineItems_;
    QPointF lastClickPos_;
};
