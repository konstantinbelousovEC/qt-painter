// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include "../include/drawing-graphics-view.h"
#include "../include/constants.h"

namespace {
    constexpr Qt::GlobalColor kDefaultColor{Qt::black};
}

DrawingGraphicsView::DrawingGraphicsView(QGraphicsScene* scene, int strokeWidth)
    : QGraphicsView(scene),
      fillColor_(kDefaultColor),
      strokeColor_(kDefaultColor),
      strokeWidth_(strokeWidth)
{
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
}

DrawingGraphicsView::~DrawingGraphicsView() = default;

QColor DrawingGraphicsView::getFillColor() const noexcept {
    return fillColor_;
}

QColor DrawingGraphicsView::getStrokeColor() const noexcept {
    return strokeColor_;
}

int DrawingGraphicsView::getStrokeWidth() const noexcept {
    return static_cast<int>(strokeWidth_);
}

void DrawingGraphicsView::setFillColor(const QColor& color) {
    fillColor_ = color;
}

void DrawingGraphicsView::setStrokeColor(const QColor& color) {
    strokeColor_ = color;
}

void DrawingGraphicsView::setStrokeWidth(int width) {
    assert(width >= 0); // this condition must be guaranteed by QSpinBox constraints of minimum value;
    strokeWidth_ = width;
}
