// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QGraphicsLineItem>
#include "../include/polygon-mode-view.h"
#include "../include/constants.h"

PolygonModeView::PolygonModeView(QGraphicsScene* scene)
    : Polygon(scene, constants::kDefaultColor, constants::kDefaultColor, constants::kDefaultStrokeWidth) {}

void PolygonModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
            lastClickPos_ = mapToScene(event->pos());
            auto* tmpLinePointer =
                    scene()->addLine(QLineF{lastClickPos_, lastClickPos_});

            if (tmpLinePointer == nullptr) return;
            tmpLinePointer->setPen(QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin});
            points_.push_back(lastClickPos_);
            lineItems_.push_back(tmpLinePointer);
    } else if (event->button() == Qt::RightButton) {
        deleteTemporaryLines();
        lastClickPos_ = mapToScene(event->pos());
        points_.push_back(lastClickPos_);
        createPolygon();
        emit changeStateOfScene();
    }
}

void PolygonModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentClickPos = mapToScene(event->pos());
    if (lineItems_.empty()) return;
    lineItems_.back()->setLine(QLineF{lastClickPos_, currentClickPos});
}
