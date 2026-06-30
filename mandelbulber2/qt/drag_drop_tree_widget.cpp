
/*
 * drag_drop_tree_widget.cpp
 *
 * Custom QTreeWidget with drag-and-drop support for object tree nodes.
 * Uses Qt6 signals/slots to communicate drag state to the parent widget.
 */

#include "drag_drop_tree_widget.h"
#include <QApplication>

cDragDropTreeWidget::cDragDropTreeWidget(QWidget *parent)
		: QTreeWidget(parent), m_sourceNodeId(-1), m_dragActive(false)
{
}

void cDragDropTreeWidget::mousePressEvent(QMouseEvent *event)
{
	m_dragStartPosition = event->pos();
	QTreeWidget::mousePressEvent(event);
}

void cDragDropTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		int distance = (event->pos() - m_dragStartPosition).manhattanLength();
		if (distance >= QApplication::startDragDistance())
		{
			QTreeWidgetItem *item = itemAt(event->pos());
			if (item)
			{
				startDrag(Qt::MoveAction);
			}
		}
	}
	QTreeWidget::mouseMoveEvent(event);
}

void cDragDropTreeWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
	QTreeWidgetItem *item = currentItem();
	if (!item) return;

	emit dragStartRequested(-1, item);

	// If dragStartRequested didn't set m_sourceNodeId, abort
	if (m_sourceNodeId <= 0) return;

	m_dragActive = true;

	QMimeData *mimeData = new QMimeData();
	mimeData->setData(kDragDropMimeFormat, QByteArray::number(m_sourceNodeId));

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->exec(Qt::MoveAction, Qt::MoveAction);

	m_dragActive = false;
	m_sourceNodeId = -1;
}

void cDragDropTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat(kDragDropMimeFormat))
	{
		event->acceptProposedAction();
	}
}

void cDragDropTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasFormat(kDragDropMimeFormat) && m_dragActive)
	{
		QPoint pos = event->position().toPoint();
		QTreeWidgetItem *targetItem = itemAt(pos);
		if (!targetItem)
		{
			event->ignore();
			return;
		}

		DropPosition dropPos = calculateDropPosition(targetItem, pos);
		emit dragMoveOverItem(targetItem, static_cast<int>(dropPos), m_sourceNodeId);

		// If the parent accepted the move (by not ignoring), accept it
		event->acceptProposedAction();
	}
	else
	{
		event->ignore();
	}
}

void cDragDropTreeWidget::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat(kDragDropMimeFormat) && m_dragActive)
	{
		QPoint pos = event->position().toPoint();
		QTreeWidgetItem *targetItem = itemAt(pos);
		if (targetItem)
		{
			DropPosition dropPos = calculateDropPosition(targetItem, pos);
			emit dropCompleted(m_sourceNodeId, targetItem, static_cast<int>(dropPos));
			event->acceptProposedAction();
		}
		else
		{
			event->ignore();
		}
	}
	else
	{
		event->ignore();
	}

	m_dragActive = false;
	m_sourceNodeId = -1;
}

void cDragDropTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	QTreeWidget::dragLeaveEvent(event);
	m_dragActive = false;
	m_sourceNodeId = -1;
}

cDragDropTreeWidget::DropPosition cDragDropTreeWidget::calculateDropPosition(
	QTreeWidgetItem *targetItem, const QPoint &pos) const
{
	if (!targetItem) return DropOnViewport;

	QRect itemRect = visualRect(indexFromItem(targetItem));
	if (!itemRect.isValid()) return DropOnViewport;

	int relY = pos.y() - itemRect.top();
	int itemHeight = itemRect.height();

	if (relY < itemHeight / 3) return DropAboveItem;
	if (relY > itemHeight * 2 / 3) return DropBelowItem;
	return DropOnItem;
}
