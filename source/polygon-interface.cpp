// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include "../include/polygon-interface.h"
#include "../include/detail.h"

Polygon::Polygon(QGraphicsScene* scene, const QColor& fillColor, const QColor& strokeColor, int strokeWidth)
        : DrawingGraphicsView(scene, fillColor, strokeColor, strokeWidth),
          lastClickPos_(detail::kZeroPointF)
{
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
}

void Polygon::deleteTemporaryLines() {
    for (auto* lineItem : lineItems_) {
        scene()->removeItem(lineItem);
        delete lineItem;
    }
    lineItems_.clear();
}

void Polygon::addNewTemporaryLine(QMouseEvent *event) {
    lastClickPos_ = mapToScene(event->pos());
    auto* tmpLinePointer =
            scene()->addLine(QLineF{lastClickPos_, lastClickPos_});

    if (tmpLinePointer == nullptr) return;
    tmpLinePointer->setPen(QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin});
    points_.push_back(lastClickPos_);
    lineItems_.push_back(tmpLinePointer);
}

void Polygon::createPolygon() {
    QPolygonF polygon;
    for (const auto& point : points_) {
        polygon << point;
    }
    auto* polygonItem = scene()->addPolygon(polygon,
                                            QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin},
                                            QBrush{fillColor_.value()});

    if (polygonItem == nullptr) return;
    detail::makeItemSelectableAndMovable(polygonItem);
    points_.clear();
}