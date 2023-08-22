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

    template<typename GraphicScene>
    void deleteSelectedItems(GraphicScene* scene) {
        foreach (QGraphicsItem *item, scene->selectedItems()) {
            scene->removeItem(item);
            delete item;
        }
    }

    inline QRectF updateRectangleSize(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept {
        qreal currentWidth = currentCursorPos.x() - startCursorPos.x();
        qreal currentHeight = currentCursorPos.y() - startCursorPos.y();
        QRectF rectangle{startCursorPos, QSizeF{currentWidth, currentHeight}};
        return rectangle;
    }

}
