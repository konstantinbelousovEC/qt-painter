// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "custom-graphics-view.h"

class Polygon : public CustomGraphicsView {
 public:
    Polygon(QGraphicsScene* scene, const QColor& fillColor, const QColor& strokeColor, int strokeWidth);

 protected:
    void addNewTemporaryLine(QMouseEvent* event);
    void createPolygon();
    void deleteTemporaryLines();

    QList<QPointF> points_;
    QList<QGraphicsLineItem*> lineItems_;
    QPointF lastClickPos_;
};
