// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QGraphicsLineItem>
#include "../include/polygon-mode-view.h"
#include "../include/graphics-items-detail.h"

PolygonModeView::PolygonModeView(QGraphicsScene* scene, QSize viewSize)
    : DrawingGraphicsView(scene, viewSize) {}

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
    QPointF currentCursorPos = mapToScene(event->pos());
    emit cursorPositionChanged(currentCursorPos);
    if (lineItems_.empty()) return;
    lineItems_.back()->setLine(QLineF{lastClickPos_, currentCursorPos});
}

void PolygonModeView::deleteTemporaryLines() {
    for (auto* lineItem : lineItems_) {
        scene()->removeItem(lineItem);
        delete lineItem;
    }
    lineItems_.clear();
}

void PolygonModeView::createPolygon() {
    QPolygonF polygon;
    for (const auto& point : points_) {
        polygon << point;
    }
    auto* polygonItem = scene()->addPolygon(polygon,
                                            QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin},
                                            QBrush{fillColor_});

    if (polygonItem == nullptr) return;
    detail::makeItemSelectableAndMovable(polygonItem);
    points_.clear();
}
