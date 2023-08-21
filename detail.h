// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsItem>

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

}
