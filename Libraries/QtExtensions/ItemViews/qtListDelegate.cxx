/*ckwg +5
 * Copyright 2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include "qtListDelegate.h"

#include <QEvent>
#include <QListWidget>

//-----------------------------------------------------------------------------
qtListDelegate::qtListDelegate(QObject* parent)
  : qtAbstractListDelegate(parent)
{
}

//-----------------------------------------------------------------------------
qtListDelegate::~qtListDelegate()
{
}

//-----------------------------------------------------------------------------
QWidget* qtListDelegate::createListEditor(QWidget* parent) const
{
  // Create list widget
  QListWidget* list = new QListWidget(parent);
  connect(list, SIGNAL(currentRowChanged(int)),
          this, SLOT(editorValueChanged()));

  // Fill list widget
  QStringList names = this->valueNames();
  foreach (QString name, names)
    list->addItem(this->createListItem(name, this->valueData(name)));

  // Set a reasonable minimum size
  int rowHeight = list->sizeHintForRow(0);
  int showRows = qMin(names.count(), 6);
  int margin = 4 * list->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
  list->setMinimumHeight((showRows * rowHeight) + margin + 2);
  list->setMinimumWidth(list->minimumSizeHint().width());

  // Done
  return list;
}

//-----------------------------------------------------------------------------
QListWidgetItem* qtListDelegate::createListItem(
  const QString& name, const QVariant& data) const
{
  QListWidgetItem* item = new QListWidgetItem;
  item->setText(name);
  item->setData(Qt::UserRole, data);
  return item;
}

//-----------------------------------------------------------------------------
void qtListDelegate::setListEditorData(
  QWidget* editor, const QVariant& newData) const
{
  QListWidget* list = qobject_cast<QListWidget*>(editor);

  int i = -1;
  for (int j = 0; j < list->count(); ++j)
    {
    if (this->compareData(newData, list->item(j)->data(Qt::UserRole)))
      {
      i = j;
      break;
      }
    }
  list->setCurrentRow(i);
}

//-----------------------------------------------------------------------------
void qtListDelegate::setModelData(
  QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  QListWidget* list = qobject_cast<QListWidget*>(editor);
  QListWidgetItem* item = list->currentItem();
  if (item)
    {
    this->setModelData(editor, item->text(), item->data(Qt::UserRole),
                       model, index);
    }
}

//-----------------------------------------------------------------------------
void qtListDelegate::updateEditorGeometry(
  QWidget* editor, const QStyleOptionViewItem& opt, const QModelIndex&) const
{
  QWidget* parent = editor->parentWidget();
  QRect geom(QPoint(0, 0), parent->size());
  QRect rect = opt.rect;

  // Grow rect to fit editor
  rect = rect.united(QRect(rect.topLeft(), editor->minimumSize()));

  // Translate to fit within parent
  if (geom.top() > rect.top())
    {
    rect.moveTop(geom.top());
    }
  if (geom.left() > rect.left())
    {
    rect.moveLeft(geom.left());
    }
  if (geom.bottom() < rect.bottom())
    {
    rect.moveBottom(geom.bottom());
    }
  if (geom.right() < rect.right())
    {
    rect.moveRight(geom.right());
    }

  // Reduce size to fit within parent (in case editor's size > parent's size)
  rect = rect.intersected(geom);

  // Set editor geometry
  editor->setFixedSize(rect.size()); // Override minimum size
  editor->setGeometry(rect);
}
