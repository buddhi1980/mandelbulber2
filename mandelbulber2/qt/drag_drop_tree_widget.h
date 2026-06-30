
/*
 * drag_drop_tree_widget.h
 *
 * Custom QTreeWidget with drag-and-drop support for object tree nodes.
 * Uses Qt6 signals/slots to communicate drag state to the parent widget.
 */

#ifndef MANDELBULBER2_QT_DRAG_DROP_TREE_WIDGET_H_
#define MANDELBULBER2_QT_DRAG_DROP_TREE_WIDGET_H_

#include <QTreeWidget>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

static constexpr const char *kDragDropMimeFormat = "application/x-mandelbulber-nodeid";

class cDragDropTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	explicit cDragDropTreeWidget(QWidget *parent = nullptr);

	void setSourceNodeId(int nodeId) { m_sourceNodeId = nodeId; }
	int sourceNodeId() const { return m_sourceNodeId; }

	enum DropPosition
	{
		DropOnViewport = 0,
		DropAboveItem,
		DropOnItem,
		DropBelowItem,
	};

signals:
	// Emitted when a drag operation is about to start. Return true to allow the drag.
	void dragStartRequested(int nodeId, QTreeWidgetItem *item);

	// Emitted when the mouse moves during a drag. Check if drop is valid.
	// Return true via the signal parameter to accept the drop indicator.
	void dragMoveOverItem(QTreeWidgetItem *targetItem, int dropPosition, int sourceNodeId);

	// Emitted when a valid drop occurs.
	void dropCompleted(int nodeId, QTreeWidgetItem *targetItem, int dropPosition);

	// Emitted after a successful drop to trigger a tree refresh.
	void treeStructureChanged();

public:
	// Call this from onDragMoveOverItem to update whether drop is valid.
	// The drop indicator will only show when isValidDrop is true.
	void setDropValid(bool isValidDrop);

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void startDrag(Qt::DropActions supportedActions) override;
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

private:
	DropPosition calculateDropPosition(QTreeWidgetItem *targetItem, const QPoint &pos) const;

	QPoint m_dragStartPosition;
	int m_sourceNodeId;
	bool m_dragActive;
	bool m_dropValid;

	// Drop indicator line tracking
	QTreeWidgetItem *m_dropTargetItem = nullptr;
	DropPosition m_dropTargetPosition = DropOnViewport;
};

#endif /* MANDELBULBER2_QT_DRAG_DROP_TREE_WIDGET_H_ */
