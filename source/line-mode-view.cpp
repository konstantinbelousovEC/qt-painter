// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QGraphicsLineItem>
#include <QMouseEvent>
#include "../include/line-mode-view.h"
#include "../include/detail.h"

namespace {
    constexpr Qt::GlobalColor kDefaultLineFillColor{Qt::blue};
    constexpr int kDefaultLineWidth{5};
}

LineModeView::LineModeView(QGraphicsScene *scene)
    : DrawingGraphicsView(scene, std::nullopt, kDefaultLineFillColor, kDefaultLineWidth),
      currentItem_(nullptr),
      startCursorPos_(detail::kZeroPointF)
{
    setRenderHint(QPainter::Antialiasing);
}

void LineModeView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addLine(QLineF{startCursorPos_,
                                               startCursorPos_},
                                        QPen{strokeColor_.value(),
                                             static_cast<qreal>(strokeWidth_),
                                             Qt::SolidLine,
                                             Qt::SquareCap,
                                             Qt::MiterJoin});

        if (currentItem_ != nullptr)
            detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void LineModeView::mouseMoveEvent(QMouseEvent *event) {
    if (currentItem_ != nullptr && (event->buttons() & Qt::LeftButton)) {
        QPointF currentCursorPos = mapToScene(event->pos());
        currentItem_->setLine(QLineF{startCursorPos_, currentCursorPos});
        emit changeStateOfScene();
    }
}

void LineModeView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (currentItem_->line().p1() == currentItem_->line().p2()) {
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}
