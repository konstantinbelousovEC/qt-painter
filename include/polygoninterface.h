// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include "detail.h"

class Polygon : public QGraphicsView {
 public:
    explicit Polygon(QGraphicsScene* scene, QColor fillColor, QColor strokeColor);

 protected:
    void addNewTemporaryLine(QMouseEvent* event);
    void createPolygon();
    void deleteTemporaryLines();

    QVector<QPointF> points_;
    QVector<QGraphicsLineItem*> lineItems_;
    QPointF lastClickPos_;
    QColor fillColor_;
    QColor strokeColor_;
};
