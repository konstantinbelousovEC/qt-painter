// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsItem>
#include "constants.h"

namespace detail {

    template<typename ItemType>
    void makeItemSelectableAndMovable(ItemType* item) {
        item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        item->setFlag(QGraphicsItem::ItemIsMovable, true);
    }

    template<typename GraphicScene, typename ItemType>
    void deleteItem(GraphicScene* scene, ItemType* item) {
        scene->removeItem(item);
        delete item;
    }

    template<typename GraphicsItem>
    bool shouldDeleteZeroSizeItem(const GraphicsItem* currentItem,
                                  const QPointF& startPos) {
        auto rect = currentItem->rect();
        return rect.topLeft() == startPos && rect.size() == constants::kZeroSizeF;
    }

}  // namespace detail
