// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QApplication>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <qmath.h>
#include <stdexcept>
#include "../include/modificationmodeview.h"
#include "../include/detail.h"

namespace {
    const QColor kSelectionAreaBrush{0, 0, 200, 15};
    const QColor kSelectionAreaPen{0 , 0, 255};
    constexpr QRectF kZeroSizeFRectangle{0, 0, 0, 0};
    constexpr qreal kSelectionAreaZValue{1.0};
    constexpr qreal kZeroAngle{0};

    enum class GraphicItemType {
        kRect,
        kCircle,
        kTriangle,
        kPath
    };

    enum class CoordsType {
        kSceneCoords,
        kItemCoords
    };
}  // namespace

template<typename ItemT, typename ItemU>
void copyGraphicProperties(const ItemT* originalItem, ItemU* destinationElement);

template<GraphicItemType type, typename ItemType>
auto copyGraphicsItem(ItemType* originalItem);

QPointF getGraphicsItemSceneCenterPos(const QGraphicsItem* item);
QPointF getGraphicsItemOwnCenterPos(const QGraphicsItem* item);
QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene);
QGraphicsItem* cloneGraphicsItem(QGraphicsItem* originalItem);
qreal calculateRotationAngle(const QPointF& O, const QPointF& A, const QPointF& B) noexcept;
inline QGraphicsItem* getItemUnderCursor(const QGraphicsScene* scene, const QPointF& currentCursorPos);
void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items);

// ------------------------------------------------------------------------------------------------------------

ModificationModeView::ModificationModeView(QGraphicsScene* graphic_scene)
    : QGraphicsView(graphic_scene),
      selectionArea_(new QGraphicsRectItem()),
      selectionStartPos_(detail::kZeroPointF),
      lastClickPos_(detail::kZeroPointF),
      rotationPointA_(detail::kZeroPointF),
      isMoving_(false)
{
    setSelectionAreaProperties();
}

void ModificationModeView::mousePressEvent(QMouseEvent* event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    QGraphicsItem* itemUnderCursor =
            getItemUnderCursor(scene(), currentCursorPos);

    if (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier) {
        handleMiddleButtonClick(itemUnderCursor, currentCursorPos);
    } else if (event->button() == Qt::LeftButton) {
        handleLeftButtonClick(event, itemUnderCursor, currentCursorPos);
    } else if (event->button() == Qt::RightButton) {
        handleRightButtonClick(event, itemUnderCursor);
    }
}

inline bool isSelectionEvent(const QMouseEvent* event) noexcept {
    return event->buttons() & Qt::LeftButton &&
           event->modifiers() ^ Qt::ShiftModifier;
}

void ModificationModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF mouseCurrentPos = mapToScene(event->pos());
    if (event->buttons() & Qt::RightButton) {
        if (rotationInfo_.isEmpty()) rotationInfo_.fillInfo(scene()->selectedItems());
        rotateSelectedItems(event);
        emit changeStateOfScene();
    } else if (isMoving_) {
        if (event->buttons() & Qt::LeftButton) moveSelectedItems(mouseCurrentPos);
        emit changeStateOfScene();
    } else if (isSelectionEvent(event)) {
        updateSelectionArea(event, mouseCurrentPos);
    }
}

void ModificationModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) rotationInfo_.clear();
    isMoving_ = false;
    selectionArea_->setRect(kZeroSizeFRectangle);
    selectionArea_->hide();
}

void ModificationModeView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) {
        detail::deleteSelectedItems(scene());
        emit changeStateOfScene();
    }
}

void ModificationModeView::handleLeftButtonClick(QMouseEvent* event,
                                                 QGraphicsItem* itemUnderCursor,
                                                 const QPointF& currentCursorPos) {
    if (itemUnderCursor == nullptr) {
        QGraphicsView::mousePressEvent(event);
        if (event->modifiers() ^ Qt::ControlModifier) scene()->clearSelection();
        selectionStartPos_ = currentCursorPos;
    } else {
        if (event->modifiers() & Qt::ControlModifier) {
            itemUnderCursor->setSelected(!itemUnderCursor->isSelected());
        } else if (!itemUnderCursor->isSelected()) {
            scene()->clearSelection();
            itemUnderCursor->setSelected(true);
        }
        isMoving_ = true;
        lastClickPos_ = currentCursorPos;
    }
}

void ModificationModeView::handleMiddleButtonClick(QGraphicsItem* itemUnderCursor,
                                                   const QPointF& currentCursorPos) {
    if (itemUnderCursor != nullptr) {
        QList<QGraphicsItem*> clonedItems = cloneSelectedItems(scene());
        updateSceneSelection(scene(), clonedItems);
        isMoving_ = true;
        lastClickPos_ = currentCursorPos;
    }
}

void ModificationModeView::handleRightButtonClick(QMouseEvent* event,
                                                  QGraphicsItem* itemUnderCursor) {
    if (itemUnderCursor != nullptr) {
        rotationPointA_ = mapToScene(event->pos());
    } else {
        scene()->clearSelection();
    }
}

void ModificationModeView::updateSelectionArea(QMouseEvent* event,
                                               const QPointF& mouseCurrentPos) {
    selectionArea_->show();
    QRectF updatedRectangle =
            detail::updateRectangleSize(selectionStartPos_, mouseCurrentPos);

    selectionArea_->setRect(updatedRectangle.normalized());
    updateItemsSelection(event, selectionArea_->rect());
}

void ModificationModeView::updateItemsSelection(QMouseEvent* event,
                                                const QRectF &selectionRectangle) {
    QList<QGraphicsItem*> itemsInRect = scene()->items(selectionRectangle);
    foreach(QGraphicsItem *item, scene()->items()) {
        if (event->modifiers() ^ Qt::ControlModifier) {
            item->setSelected(itemsInRect.contains(item));
        } else {
            item->setSelected(item->isSelected() || itemsInRect.contains(item));
        }
    }
}

void ModificationModeView::moveSelectedItems(const QPointF& mouseCurrentPos) {
    QPointF delta = mouseCurrentPos - lastClickPos_;
    foreach(QGraphicsItem* item, scene()->selectedItems()) {
        item->moveBy(delta.x(), delta.y());
    }
    lastClickPos_ = mouseCurrentPos;
}

void ModificationModeView::rotateSelectedItems(QMouseEvent* event) {
    const auto& items = rotationInfo_.getItems();
    const auto& angles = rotationInfo_.getAngles();
    for (int i = 0; i < rotationInfo_.size(); i++) {
        rotateItem(event, items[i], angles[i]);
    }
}

void ModificationModeView::rotateItem(QMouseEvent *event, QGraphicsItem* item, qreal startAngle) {
    QPointF pointO = getGraphicsItemSceneCenterPos(item);
    QPointF pointB = mapToScene(event->pos());
    qreal rotationAngle = calculateRotationAngle(pointO,
                                                 rotationPointA_,
                                                 pointB);

    item->setTransformOriginPoint(getGraphicsItemOwnCenterPos(item));
    item->setRotation(startAngle + rotationAngle);
}

void ModificationModeView::setSelectionAreaProperties() {
    selectionArea_->setPen(QPen{kSelectionAreaPen});
    selectionArea_->setBrush(QBrush{kSelectionAreaBrush});
    selectionArea_->hide();
    selectionArea_->setFlag(QGraphicsItem::ItemIsSelectable, false);
    selectionArea_->setFlag(QGraphicsItem::ItemIsMovable, false);
    selectionArea_->setZValue(kSelectionAreaZValue);
    scene()->addItem(selectionArea_);
}

// ------------------------------------------------------------------------------------------------------------

template<CoordsType type>
QPointF getPolygonCenterRelativeTo(const QGraphicsPolygonItem* polygonItem) {
    auto points = polygonItem->polygon().toVector();
    QPointF sum{};

    foreach(const auto& point, points) {
        if constexpr (type == CoordsType::kSceneCoords) {
            sum += polygonItem->mapToScene(point);
        } else if constexpr (type == CoordsType::kItemCoords) {
            sum += point;
        }
    }

    return sum / static_cast<qreal>(points.size());
}

using RectItem_p = const QGraphicsRectItem*;
using EllipseItem_p = const QGraphicsEllipseItem*;
using PolygonItem_p = const QGraphicsPolygonItem*;
using PathItem_p = const QGraphicsPathItem*;

QPointF getGraphicsItemSceneCenterPos(const QGraphicsItem* item) {
    if (const auto* rectItem = qgraphicsitem_cast<RectItem_p>(item)) {
        return rectItem->sceneBoundingRect().center();
    } else if (const auto* ellipseItem = qgraphicsitem_cast<EllipseItem_p>(item)) {
        return ellipseItem->sceneBoundingRect().center();
    } else if (const auto* triangleItem = qgraphicsitem_cast<PolygonItem_p>(item)) {
        return getPolygonCenterRelativeTo<CoordsType::kSceneCoords>(triangleItem);
    } else if (const auto* pathItem = qgraphicsitem_cast<PathItem_p>(item)) {
        return pathItem->sceneBoundingRect().center();
    } else {
        throw std::runtime_error("Can not define figure's center: unknown figure type");
    }
}

QPointF getGraphicsItemOwnCenterPos(const QGraphicsItem* item) {
    if (const auto* rectItem = qgraphicsitem_cast<RectItem_p>(item)) {
        return rectItem->boundingRect().center();
    } else if (const auto* ellipseItem = qgraphicsitem_cast<EllipseItem_p>(item)) {
        return ellipseItem->boundingRect().center();
    } else if (const auto* triangleItem = qgraphicsitem_cast<PolygonItem_p>(item)) {
        return getPolygonCenterRelativeTo<CoordsType::kItemCoords>(triangleItem);
    } else if (const auto* pathItem = qgraphicsitem_cast<PathItem_p>(item)) {
        return pathItem->boundingRect().center();
    } else {
        throw std::runtime_error("Can not define figure's center: unknown figure type");
    }
}

template<typename ItemT, typename ItemU>
void copyGraphicProperties(const ItemT* originalItem, ItemU* destinationElement) {
    destinationElement->setPen(originalItem->pen());
    destinationElement->setBrush(originalItem->brush());
}

template<typename ItemType, typename AngleT>
inline void setUpItemsAngles(ItemType* originalItem, ItemType* destinationItem, AngleT angle) {
    destinationItem->setTransformOriginPoint(getGraphicsItemSceneCenterPos(destinationItem));
    destinationItem->setRotation(angle);
    originalItem->setRotation(angle);
    copyGraphicProperties(originalItem, destinationItem);
}

template<typename ItemType>
auto copyGraphicsItem(ItemType* originalItem) {
    auto rotationAngle = originalItem->rotation();
    originalItem->setRotation(kZeroAngle);
    ItemType* temporaryItem{nullptr};

    if constexpr (std::is_same_v<ItemType, QGraphicsRectItem> || std::is_same_v<ItemType, QGraphicsEllipseItem>) {
        temporaryItem = new ItemType{originalItem->sceneBoundingRect()};
    } else if constexpr (std::is_same_v<ItemType, QGraphicsPolygonItem>) {
        temporaryItem = new ItemType{originalItem->mapToScene(originalItem->polygon())};
    } else if constexpr (std::is_same_v<ItemType, QGraphicsPathItem>) {
        temporaryItem = new ItemType{originalItem->mapToScene(originalItem->path())};
    }

    if (temporaryItem != nullptr)
        setUpItemsAngles(originalItem, temporaryItem, rotationAngle);

    return temporaryItem;
}

QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene) {
    QList<QGraphicsItem*> clonedItems;
    foreach(QGraphicsItem* item, scene->selectedItems()) {
        QGraphicsItem* clonedItem = cloneGraphicsItem(item);
        if (clonedItem) {
            clonedItems.append(clonedItem);
            scene->addItem(clonedItem);
        }
    }
    return clonedItems;
}

QGraphicsItem* cloneGraphicsItem(QGraphicsItem* originalItem) {
    QGraphicsItem* copiedItem = nullptr;

    if (auto* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(originalItem)) {
        copiedItem = copyGraphicsItem(rectItem);
    } else if (auto* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(originalItem)) {
        copiedItem = copyGraphicsItem(ellipseItem);
    } else if (auto* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(originalItem)) {
        copiedItem = copyGraphicsItem(polygonItem);
    } else if (auto* pathItem = qgraphicsitem_cast<QGraphicsPathItem*>(originalItem)) {
        copiedItem = copyGraphicsItem(pathItem);
    }

    if (copiedItem != nullptr)
        detail::makeItemSelectableAndMovable(copiedItem);

    return copiedItem;
}

qreal calculateRotationAngle(const QPointF& O,
                             const QPointF& A,
                             const QPointF& B) noexcept {
    qreal angleAO = qAtan2(A.y() - O.y(), A.x() - O.x());
    qreal angleBO = qAtan2(B.y() - O.y(), B.x() - O.x());
    qreal angle = angleBO - angleAO;

    if (angle < 0) angle += 2 * M_PI;

    return qRadiansToDegrees(angle);
}

inline QGraphicsItem* getItemUnderCursor(const QGraphicsScene* scene, const QPointF& currentCursorPos) {
    return scene->itemAt(currentCursorPos, QTransform{});
}

void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items) {
    scene->clearSelection();
    foreach(QGraphicsItem* item, items) {
        item->setSelected(true);
    }
}

void ModificationModeView::RotationInfo::clear() {
    items_.clear();
    angles_.clear();
}

qsizetype ModificationModeView::RotationInfo::size() const noexcept {
    return items_.size();
}

bool ModificationModeView::RotationInfo::isEmpty() const noexcept {
    return items_.isEmpty();
}

bool ModificationModeView::RotationInfo::fillInfo(const QList<QGraphicsItem *> &items) {
    items_ = items;
    angles_.reserve(items_.size());
    foreach(const auto& item, items_) {
        angles_.push_back(item->rotation());
    }
    return items_.size() == angles_.size();
}

const QList<QGraphicsItem*>& ModificationModeView::RotationInfo::getItems() const noexcept {
    return items_;
}

const QList<qreal>& ModificationModeView::RotationInfo::getAngles() const noexcept {
    return angles_;
}
