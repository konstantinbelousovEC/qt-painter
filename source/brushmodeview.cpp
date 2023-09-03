// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QPainterPath>
#include <QGraphicsEllipseItem>
#include "../include/brushmodeview.h"
#include "../include/detail.h"

namespace {
    constexpr int kDefaultBrushWidth{10};
    constexpr Qt::GlobalColor kDefaultBrushFillColor{Qt::red};
    const QPen kDefaultPen{kDefaultBrushFillColor, kDefaultBrushWidth, Qt::SolidLine, Qt::RoundCap};
}

BrushModeView::BrushModeView(QGraphicsScene* scene)
        : QGraphicsView(scene),
          startEllipseItem_(nullptr),
          startCursorPos_(detail::kZeroPointF),
          previousCursorPos_(detail::kZeroPointF),
          lineColor_(kDefaultBrushFillColor),
          lineWidth_(kDefaultBrushWidth)
{
    setRenderHint(QPainter::Antialiasing);
}

void BrushModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        startEllipseItem_ = scene()->addEllipse(startCursorPos_.x() - lineWidth_ / 2.0,
                                                startCursorPos_.y() - lineWidth_ / 2.0,
                                                lineWidth_,
                                                lineWidth_,
                                                QPen{Qt::NoPen},
                                                QBrush{lineColor_});

        if (startEllipseItem_ != nullptr)
            detail::makeItemSelectableAndMovable(startEllipseItem_);

        previousCursorPos_ = startCursorPos_;
    }
}

void BrushModeView::mouseMoveEvent(QMouseEvent* event) {
    if (startEllipseItem_ != nullptr && event->buttons() & Qt::LeftButton) {
        QPointF currentCursorPos = mapToScene(event->pos());
        auto* currentTemporaryLine = scene()->addLine(previousCursorPos_.x(),
                                                      previousCursorPos_.y(),
                                                      currentCursorPos.x(),
                                                      currentCursorPos.y(),
                                                      kDefaultPen);

        if (currentTemporaryLine != nullptr) {
            temporaryLines_.push_back(currentTemporaryLine);
            previousCursorPos_ = currentCursorPos;
        }
    }
}

void BrushModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (startEllipseItem_ != nullptr && event->button() == Qt::LeftButton) {
        if (!temporaryLines_.empty()) {
            QPainterPath path;
            path.moveTo(startCursorPos_);

            foreach(auto* line, temporaryLines_) {
                path.moveTo(line->line().p1());
                path.lineTo(line->line().p2());
                detail::deleteItem(scene(), line);
            }
            temporaryLines_.clear();

            auto* scenePath = scene()->addPath(path, kDefaultPen);
            if (scenePath != nullptr) detail::makeItemSelectableAndMovable(scenePath);

            detail::deleteItem(scene(), startEllipseItem_);
            startEllipseItem_ = nullptr;
        }
    }
}

void BrushModeView::changeLineColor(const QColor& color) {
    lineColor_ = color;
}

void BrushModeView::changeLineWidth(int width) {
    lineWidth_ = width;
}
