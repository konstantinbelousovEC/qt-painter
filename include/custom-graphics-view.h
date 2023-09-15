// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>
#include <optional>

class DrawingGraphicsView : public QGraphicsView {
    Q_OBJECT

 public:
    DrawingGraphicsView(QGraphicsScene* scene,
                       std::optional<QColor> fillColor,
                       std::optional<QColor> strokeColor,
                       int strokeWidth);

    ~DrawingGraphicsView() override;

    [[nodiscard]] virtual QColor getFillColor() const noexcept;
    [[nodiscard]] virtual QColor getStrokeColor() const noexcept;
    [[nodiscard]] virtual int getStrokeWidth() const noexcept;

    virtual void setFillColor(const QColor& color);
    virtual void setStrokeColor(const QColor& color);
    virtual void setStrokeWidth(int width);

 protected:
    std::optional<QColor> fillColor_;
    std::optional<QColor> strokeColor_;
    int strokeWidth_;
};
