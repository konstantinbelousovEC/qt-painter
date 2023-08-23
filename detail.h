// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsItem>

namespace detail {

    constexpr QSizeF kZeroSizeF{0, 0};
    constexpr QPointF kZeroPointF{0, 0};

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

    template<typename GraphicsItem>
    bool shouldDeleteZeroSizeItem(const GraphicsItem* currentItem, const QPointF& startPos) {
        auto rect = currentItem->rect();
        return rect.topLeft() == startPos && rect.size() == kZeroSizeF;
    }

    QRectF updateRectangleSize(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept;

}
