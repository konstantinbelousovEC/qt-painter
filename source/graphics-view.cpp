// @copyright Copyright (c) 2023 by Konstantin Belousov
#include "../include/graphics-view.h"

ApplicationGraphicsView::ApplicationGraphicsView(QGraphicsScene* scene,
                                                 int viewWidth,
                                                 int viewHeight)
    : QGraphicsView(scene),
      viewWidth_(viewWidth),
      viewHeight_(viewHeight)
{
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSceneRect(0, 0, viewWidth_ + 1, viewHeight_ + 1);
    setFixedSize(viewWidth_ + 3, viewHeight_ + 3);
}
