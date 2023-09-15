// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>
#include <optional>

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT

 public:
    CustomGraphicsView(QGraphicsScene* scene,
                       std::optional<QColor> fillColor,
                       std::optional<QColor> strokeColor,
                       int strokeWidth);

    ~CustomGraphicsView() override;

    [[nodiscard]] virtual QColor getFillColor() const noexcept;
    [[nodiscard]] virtual QColor getStrokeColor() const noexcept;
    [[nodiscard]] virtual int getStrokeWidth() const noexcept;

    virtual void changeFillColor(const QColor& color);
    virtual void changeStrokeColor(const QColor& color);
    virtual void changeStrokeWidth(int width);

 protected:
    std::optional<QColor> fillColor_;
    std::optional<QColor> strokeColor_;
    int strokeWidth_;
};
