// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>
#include "polygoninterface.h"

class PolygonModeView : public Polygon {
    Q_OBJECT

 public:
    explicit PolygonModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

 private slots:
    void changeFillColor(const QColor& color);
    void changeStrokeColor(const QColor& color);

 signals:
    void changeStateOfScene();
};
