// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "../include/trianglemode.h"
#include "../include/detail.h"

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
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
    points_.reserve(3);
    lineItems_.reserve(2);
}

void TriangleModeView::mousePressEvent(QMouseEvent* event) {
    bool isSuccessfull{false};
    if (event->button() == Qt::LeftButton) {
        if (clickCount_ == 0 || clickCount_ == 1) {
            isSuccessfull = addNewTemporaryLine(event);
        } else if (clickCount_ == 2) {
            deleteTemporaryLines();
            lastClickPos_ = mapToScene(event->pos());
            points_.push_back(lastClickPos_);
            isSuccessfull = createPolygon();
        }

        if (isSuccessfull) ++clickCount_;
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
        if (lineItems_.empty()) return;
        lineItems_.back()->setLine(QLineF{lastClickPos_, currentClickPos});
    }
}

bool TriangleModeView::createPolygon() {
    QPolygonF polygon;
    foreach(const auto& point, points_) {
        polygon << point;
    }
    auto* polygonItem = scene()->addPolygon(polygon,
                                            QPen{strokeColor_},
                                            QBrush{fillColor_});

    if (polygonItem == nullptr) return false;
    detail::makeItemSelectableAndMovable(polygonItem);
    return true;
}

void TriangleModeView::deleteTemporaryLines() {
    foreach(auto* lineItem, lineItems_) {
        scene()->removeItem(lineItem);
        delete lineItem;
    }
    lineItems_.clear();
}

bool TriangleModeView::addNewTemporaryLine(QMouseEvent* event) {
    lastClickPos_ = mapToScene(event->pos());
    auto* tmpLinePointer = scene()->addLine(QLineF{lastClickPos_, lastClickPos_});
    if (tmpLinePointer == nullptr) return false;

    tmpLinePointer->setPen(QPen{strokeColor_});
    points_.push_back(lastClickPos_);
    lineItems_.push_back(tmpLinePointer);
    return true;
}

void TriangleModeView::changeFillColor(const QColor& color) {
    fillColor_ = color;
}

void TriangleModeView::changeStrokeColor(const QColor& color) {
    strokeColor_ = color;
}
