// @copyright Copyright (c) 2023 by Konstantin Belousov
#include "../include/graphics-view.h"

ApplicationGraphicsView::ApplicationGraphicsView(QGraphicsScene* scene, QSize viewSize)
    : QGraphicsView(scene),
      viewSize_(viewSize)
{
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSceneRect(0, 0, viewSize_.width() + 1, viewSize_.height() + 1);
    setFixedSize(viewSize_.width() + 3, viewSize_.height() + 3);
}
