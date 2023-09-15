// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QGraphicsLineItem>
#include "../include/trianglemode.h"

namespace {
    constexpr Qt::GlobalColor kDefaultTriangleFillColor{Qt::cyan};
    constexpr Qt::GlobalColor kDefaultTriangleStrokeColor{Qt::black};
}

TriangleModeView::TriangleModeView(QGraphicsScene* scene)
    : Polygon(scene, kDefaultTriangleFillColor, kDefaultTriangleStrokeColor, 1) {} // todo: make a new file with common constants

void TriangleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (points_.size() < 2) {
            addNewTemporaryLine(event);
        } else if (points_.size() == 2) {
            deleteTemporaryLines();
            lastClickPos_ = mapToScene(event->pos());
            points_.push_back(lastClickPos_);
            createPolygon();
        }
        emit changeStateOfScene();
    }
}

void TriangleModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentClickPos = mapToScene(event->pos());
    if (lineItems_.empty()) return;
    lineItems_.back()->setLine(QLineF{lastClickPos_, currentClickPos});
}
