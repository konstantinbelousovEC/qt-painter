// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QApplication>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <qmath.h>
#include "../include/modification-mode-view.h"
#include "../include/graphics-items-detail.h"
#include "../include/rectangles-detail.h"

namespace {
    const QColor kSelectionAreaBrush{0, 0, 200, 15};
    const QColor kSelectionAreaPen{0 , 0, 255};
    constexpr QRectF kZeroSizeFRectangle{0, 0, 0, 0};
    constexpr qreal kSelectionAreaZValue{1.0};
    constexpr qreal kZeroAngle{0};

    enum class CoordsType {
        kSceneCoords,
        kItemCoords
    };
}  // namespace

class RotationInfo {
/*
 An auxiliary class for performing rotation of graphic elements,
 storing selected scene elements and their corresponding initial rotation angles.
 If you do not store this information, then each new rotation begins without taking into account the previous one,
 which leads to a visual "jump" of the figure and does not meet the requirements of the application functionality.

 This information is filled in when the right mouse button is pressed and cleared when the right mouse button is released.
*/
 public:
    void clear();
    bool fillInfo(const QList<QGraphicsItem*>& items);
    [[nodiscard]] qsizetype size() const noexcept;
    [[nodiscard]] bool isEmpty() const noexcept;
    [[nodiscard]] const QList<QGraphicsItem*>& getItems() const noexcept;
    [[nodiscard]] const QList<qreal>& getAngles() const noexcept;

 private:
    QList<QGraphicsItem*> items_;
    QList<qreal> angles_;
};

void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items);
QPointF getGraphicsItemSceneCenterPos(const QGraphicsItem* item);
QPointF getGraphicsItemOwnCenterPos(const QGraphicsItem* item);
QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene);
qreal calculateRotationAngle(const QPointF& geometricCenterO,
                             const QPointF& initialCursorPosA,
                             const QPointF& currentCursorPosB) noexcept;

// ------------------------------------------------------------------------------------------------------------

ModificationModeView::ModificationModeView(QGraphicsScene* graphic_scene, QSize viewSize)
    : ApplicationGraphicsView(graphic_scene, viewSize),
      selectionArea_(new QGraphicsRectItem()),
      rotationInfo_(std::make_unique<RotationInfo>()),
      selectionStartPos_(constants::kZeroPointF),
      lastClickPos_(constants::kZeroPointF),
      initialCursorPosA_(constants::kZeroPointF),
      isMoving_(false)
{
    setSelectionAreaProperties();
}

void ModificationModeView::mousePressEvent(QMouseEvent* event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    QGraphicsItem* itemUnderCursor = scene()->itemAt(currentCursorPos, QTransform{});

    if (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier) {
        handleMiddleButtonClick(itemUnderCursor, currentCursorPos);
    } else if (event->button() == Qt::LeftButton) {
        handleLeftButtonClick(event, itemUnderCursor, currentCursorPos);
    } else if (event->button() == Qt::RightButton) {
        handleRightButtonClick(event, itemUnderCursor);
    }
}

inline bool isSelectionEvent(const QMouseEvent* event) {
    return event->buttons() & Qt::LeftButton &&
           event->modifiers() ^ Qt::ShiftModifier;
}

void ModificationModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF mouseCurrentPos = mapToScene(event->pos());
    emit cursorPositionChanged(mouseCurrentPos);
    if (event->buttons() & Qt::RightButton) {
        if (rotationInfo_->isEmpty()) rotationInfo_->fillInfo(scene()->selectedItems());
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
    if (event->button() == Qt::RightButton) rotationInfo_->clear();
    isMoving_ = false;
    selectionArea_->setRect(kZeroSizeFRectangle);
    selectionArea_->hide();
}

template<typename GraphicScene>
void deleteSelectedItems(GraphicScene* scene) {
    for (auto* item : scene->selectedItems()) {
        scene->removeItem(item);
        delete item;
    }
}

void ModificationModeView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) {
        deleteSelectedItems(scene());
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
        initialCursorPosA_ = mapToScene(event->pos());
    } else {
        scene()->clearSelection();
    }
}

void ModificationModeView::updateSelectionArea(QMouseEvent* event,
                                               const QPointF& mouseCurrentPos) {
    selectionArea_->show();
    selectionArea_->setRect(detail::makeRectangle(selectionStartPos_,
                                                  mouseCurrentPos));
    updateItemsSelection(event, selectionArea_->rect());
}

void ModificationModeView::updateItemsSelection(QMouseEvent* event,
                                                const QRectF &selectionRectangle) {
    QList<QGraphicsItem*> itemsInRect = scene()->items(selectionRectangle);
    for (auto* item : scene()->items()) {
        if (event->modifiers() ^ Qt::ControlModifier) {
            item->setSelected(itemsInRect.contains(item));
        } else {
            item->setSelected(item->isSelected() || itemsInRect.contains(item));
        }
    }
}

void ModificationModeView::moveSelectedItems(const QPointF& mouseCurrentPos) {
    QPointF delta = mouseCurrentPos - lastClickPos_;
    for (auto* item : scene()->selectedItems()) {
        item->moveBy(delta.x(), delta.y());
    }
    lastClickPos_ = mouseCurrentPos;
}

void ModificationModeView::rotateSelectedItems(QMouseEvent* event) {
    const auto& items = rotationInfo_->getItems();
    const auto& angles = rotationInfo_->getAngles();
    for (int i = 0; i < rotationInfo_->size(); i++) {
        rotateItem(event, items[i], angles[i]);
    }
}

void ModificationModeView::rotateItem(QMouseEvent *event, QGraphicsItem* item, qreal startAngle) {
    QPointF geometricCenterO = getGraphicsItemSceneCenterPos(item);
    QPointF currentCursorPosB = mapToScene(event->pos());
    qreal rotationAngle = calculateRotationAngle(geometricCenterO,
                                                 initialCursorPosA_,
                                                 currentCursorPosB);

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

void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items) {
    scene->clearSelection();
    for (auto* item : items) {
        item->setSelected(true);
    }
}

template<CoordsType type>
QPointF getPolygonCenterRelativeTo(const QGraphicsPolygonItem* polygonItem) {
    auto points = polygonItem->polygon().toVector();
    QPointF sum{};

    for (const auto& point : points) {
        if constexpr (type == CoordsType::kSceneCoords) {
            sum += polygonItem->mapToScene(point);
        } else if constexpr (type == CoordsType::kItemCoords) {
            sum += point;
        }
    }

    return sum / static_cast<qreal>(points.size());
}

QPointF getGraphicsItemSceneCenterPos(const QGraphicsItem* item) {
    if (const auto* polygonItem = qgraphicsitem_cast<const QGraphicsPolygonItem*>(item)) {
        return getPolygonCenterRelativeTo<CoordsType::kSceneCoords>(polygonItem);
    } else {
        return item->sceneBoundingRect().center();
    }
}

QPointF getGraphicsItemOwnCenterPos(const QGraphicsItem* item) {
    if (const auto* polygonItem = qgraphicsitem_cast<const QGraphicsPolygonItem*>(item)) {
        return getPolygonCenterRelativeTo<CoordsType::kItemCoords>(polygonItem);
    } else {
        return item->boundingRect().center();
    }
}

template<typename ItemT, typename ItemU>
void copyGraphicProperties(const ItemT* originalItem, ItemU* destinationElement) {
    destinationElement->setPen(originalItem->pen());
    if constexpr (!std::is_same_v<ItemT, QGraphicsLineItem>) {
        destinationElement->setBrush(originalItem->brush());
    }
}

template<typename ItemType, typename AngleT>
inline void setUpItemsAngles(ItemType* originalItem, ItemType* destinationItem, AngleT angle) {
    destinationItem->setTransformOriginPoint(getGraphicsItemSceneCenterPos(destinationItem));
    destinationItem->setRotation(angle);
    originalItem->setRotation(angle);
    copyGraphicProperties(originalItem, destinationItem);
}

inline QLineF translateLineToSceneCoords(const QGraphicsLineItem* item) {
    QLineF newLine{item->mapToScene(item->line().p1()),
                   item->mapToScene(item->line().p2())};

    return newLine;
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
    } else if constexpr (std::is_same_v<ItemType, QGraphicsLineItem>) {
        temporaryItem = new ItemType{translateLineToSceneCoords(originalItem)};
    }

    if (temporaryItem != nullptr)
        setUpItemsAngles(originalItem, temporaryItem, rotationAngle);

    return temporaryItem;
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
    } else if (auto* lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(originalItem)) {
        copiedItem = copyGraphicsItem(lineItem);
    }

    if (copiedItem != nullptr)
        detail::makeItemSelectableAndMovable(copiedItem);

    return copiedItem;
}

QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene) {
    QList<QGraphicsItem*> clonedItems;
    for (auto* item : scene->selectedItems()) {
        QGraphicsItem* clonedItem = cloneGraphicsItem(item);
        if (clonedItem) {
            clonedItems.append(clonedItem);
            scene->addItem(clonedItem);
        }
    }
    return clonedItems;
}

qreal calculateRotationAngle(const QPointF& geometricCenterO,
                             const QPointF& initialCursorPosA,
                             const QPointF& currentCursorPosB) noexcept {
    qreal angleAO = qAtan2(initialCursorPosA.y() - geometricCenterO.y(),
                           initialCursorPosA.x() - geometricCenterO.x());
    qreal angleBO = qAtan2(currentCursorPosB.y() - geometricCenterO.y(),
                           currentCursorPosB.x() - geometricCenterO.x());

    qreal angle = angleBO - angleAO;

    if (angle < 0) angle += 2 * M_PI;

    return qRadiansToDegrees(angle);
}

// ------------------------------------------------------------------------------------------------------------

void RotationInfo::clear() {
    items_.clear();
    angles_.clear();
}

qsizetype RotationInfo::size() const noexcept {
    return items_.size();
}

bool RotationInfo::isEmpty() const noexcept {
    return items_.isEmpty();
}

bool RotationInfo::fillInfo(const QList<QGraphicsItem *> &items) {
    items_ = items;
    angles_.reserve(items_.size());
    for (const auto* item : items_) {
        angles_.push_back(item->rotation());
    }
    return items_.size() == angles_.size();
}

const QList<QGraphicsItem*>& RotationInfo::getItems() const noexcept {
    return items_;
}

const QList<qreal>& RotationInfo::getAngles() const noexcept {
    return angles_;
}
