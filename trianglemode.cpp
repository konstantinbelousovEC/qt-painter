// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "trianglemode.h"
#include "detail.h"

#include <QMouseEvent>

namespace {
    constexpr Qt::GlobalColor kDefaultTriangleFillColor{Qt::cyan};
    constexpr Qt::GlobalColor kDefaultTriangleStrokeColor{Qt::black};
}

TriangleModeView::TriangleModeView(QGraphicsScene* scene)
    : QGraphicsView(scene),
      lastClickPos_(detail::kZeroPointF),
      fillColor_(kDefaultTriangleFillColor),
      strokeColor_(kDefaultTriangleStrokeColor),
      clickCount_(0)
{
    setMouseTracking(true);
    points_.reserve(3);
    lineItems_.reserve(2);
}

void TriangleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (clickCount_ == 0 || clickCount_ == 1) {
            addNewTemporaryLine(event);
        } else if (clickCount_ == 2) {
            deleteTemporaryLines();
            lastClickPos_ = mapToScene(event->pos());
            points_.push_back(lastClickPos_);
            createPolygon();
        }

        ++clickCount_;
        if (clickCount_ > 2) {
            clickCount_ = 0;
            points_.clear();
        }
        emit changeStateOfScene();
    }
}

void TriangleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (clickCount_ == 1 || clickCount_ == 2) {
        QPointF currentClickPos = mapToScene(event->pos());
        lineItems_.back()->setLine(QLineF{lastClickPos_, currentClickPos});
    }
}

void TriangleModeView::createPolygon() {
    QPolygonF triangle;
    foreach(const auto& point, points_) {
        triangle << point;
    }
    auto* polygonItem = scene()->addPolygon(triangle,
                                            QPen{strokeColor_},
                                            QBrush{fillColor_});
    detail::makeItemSelectableAndMovable(polygonItem);
}

void TriangleModeView::deleteTemporaryLines() {
    foreach(auto* lineItem, lineItems_) {
        scene()->removeItem(lineItem);
        delete lineItem;
    }
    lineItems_.clear();
}

void TriangleModeView::addNewTemporaryLine(QMouseEvent* event) {
    lastClickPos_ = mapToScene(event->pos());
    points_.push_back(lastClickPos_);
    lineItems_.push_back(scene()->addLine(QLineF{lastClickPos_, lastClickPos_}));
    lineItems_.back()->setPen(QPen{strokeColor_});
}

void TriangleModeView::changeFillColor(const QColor& color) {
    fillColor_ = color;
}

void TriangleModeView::changeStrokeColor(const QColor& color) {
    strokeColor_ = color;
}
