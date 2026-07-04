
/*
 * drag_drop_tree_widget.cpp
 *
 * Custom QTreeWidget with drag-and-drop support for object tree nodes.
 * Uses Qt6 signals/slots to communicate drag state to the parent widget.
 */

#include "drag_drop_tree_widget.h"
#include <QApplication>
#include <QPainter>
#include <QHeaderView>

cDragDropTreeWidget::cDragDropTreeWidget(QWidget *parent)
		: QTreeWidget(parent),
			m_sourceNodeId(-1),
			m_dragActive(false),
			m_dropValid(false),
			m_dropTargetItem(nullptr),
			m_dropTargetPosition(DropOnViewport)
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		QPoint pos = event->position().toPoint();
#else
		QPoint pos = event->pos();
#endif
		QTreeWidgetItem *targetItem = itemAt(pos);
		if (!targetItem)
		{
			event->ignore();
			setDropValid(false);
			m_dropTargetItem = nullptr;
			m_dropTargetPosition = DropOnViewport;
			viewport()->update();
			return;
		}

		DropPosition dropPos = calculateDropPosition(targetItem, pos);
		m_dropTargetItem = targetItem;
		m_dropTargetPosition = dropPos;
		emit dragMoveOverItem(targetItem, static_cast<int>(dropPos), m_sourceNodeId);
		viewport()->update();

		if (m_dropValid)
		{
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
}

void cDragDropTreeWidget::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat(kDragDropMimeFormat) && m_dragActive)
	{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		QPoint pos = event->position().toPoint();
#else
		QPoint pos = event->pos();
#endif
		QTreeWidgetItem *targetItem = itemAt(pos);
		if (targetItem)
		{
			DropPosition dropPos = calculateDropPosition(targetItem, pos);
			emit dropCompleted(m_sourceNodeId, targetItem, static_cast<int>(dropPos));
			emit treeStructureChanged();
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
	m_dropValid = false;
	m_dropTargetItem = nullptr;
	m_dropTargetPosition = DropOnViewport;
	viewport()->update();
}

void cDragDropTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	QTreeWidget::dragLeaveEvent(event);
	m_dragActive = false;
	m_sourceNodeId = -1;
	m_dropValid = false;
	m_dropTargetItem = nullptr;
	m_dropTargetPosition = DropOnViewport;
	viewport()->update();
}

void cDragDropTreeWidget::paintEvent(QPaintEvent *event)
{
	QTreeWidget::paintEvent(event);

	if (!m_dragActive || !m_dropValid || !m_dropTargetItem || m_dropTargetPosition == DropOnViewport)
		return;

	QPainter painter(viewport());
	painter.setPen(QPen(Qt::blue, 2));
	painter.setRenderHint(QPainter::Antialiasing);

	QRect itemRect = visualRect(indexFromItem(m_dropTargetItem));
	int x1 = 5;
	int x2 = viewport()->width() - 5;

	if (m_dropTargetPosition == DropAboveItem)
	{
		painter.drawLine(x1, itemRect.top(), x2, itemRect.top());
	}
	else if (m_dropTargetPosition == DropBelowItem)
	{
		painter.drawLine(x1, itemRect.bottom() - 1, x2, itemRect.bottom() - 1);
	}
	else if (m_dropTargetPosition == DropOnItem)
	{
		int y = itemRect.center().y();
		painter.drawLine(x1, y, x2, y);
	}
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

void cDragDropTreeWidget::setDropValid(bool isValidDrop)
{
	if (m_dropValid != isValidDrop)
	{
		m_dropValid = isValidDrop;
		if (!isValidDrop)
		{
			m_dropTargetItem = nullptr;
			m_dropTargetPosition = DropOnViewport;
		}
		viewport()->update();
	}
}
