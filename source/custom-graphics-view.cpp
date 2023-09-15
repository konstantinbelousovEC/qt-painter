// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "../include/custom-graphics-view.h"

CustomGraphicsView::CustomGraphicsView(QGraphicsScene* scene,
                                       std::optional<QColor> fillColor,
                                       std::optional<QColor> strokeColor,
                                       int strokeWidth)
    : QGraphicsView(scene),
      fillColor_(fillColor),
      strokeColor_(strokeColor),
      strokeWidth_(strokeWidth) {}

CustomGraphicsView::~CustomGraphicsView() = default;

QColor CustomGraphicsView::getFillColor() const noexcept {
    if (fillColor_.has_value()) return fillColor_.value();
    else return QColor{qRgba(0,0,0,0)};
}

QColor CustomGraphicsView::getStrokeColor() const noexcept {
    if (strokeColor_.has_value()) return strokeColor_.value();
    else return QColor{qRgba(0,0,0,0)};
}

int CustomGraphicsView::getStrokeWidth() const noexcept {
    return strokeWidth_;
}

void CustomGraphicsView::changeFillColor(const QColor& color) {
    fillColor_ = color;
}

void CustomGraphicsView::changeStrokeColor(const QColor& color) {
    strokeColor_ = color;
}

void CustomGraphicsView::changeStrokeWidth(int width) {
    assert(width >= 0); // this condition must be guaranteed by QSpinBox constraints of minimum value;
    strokeWidth_ = width;
}
