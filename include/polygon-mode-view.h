// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "polygon-interface.h"

class PolygonModeView : public Polygon {
 public:
    explicit PolygonModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};
