// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>
#include "graphics-view.h"

class DrawingGraphicsView : public ApplicationGraphicsView {
 public:
    explicit DrawingGraphicsView(QGraphicsScene* scene, QSize viewSize, int strokeWidth = 1);

    ~DrawingGraphicsView() override;

    [[nodiscard]] virtual QColor getFillColor() const noexcept;
    [[nodiscard]] virtual QColor getStrokeColor() const noexcept;
    [[nodiscard]] virtual int getStrokeWidth() const noexcept;

    virtual void setFillColor(const QColor& color);
    virtual void setStrokeColor(const QColor& color);
    virtual void setStrokeWidth(int width);

 protected:
    QColor fillColor_;
    QColor strokeColor_;
    qreal strokeWidth_;
};
