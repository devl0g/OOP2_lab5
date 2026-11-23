#include "shapesTable.hpp"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QKeySequence>
#include <QShortcut>
#include <QtWidgets/QVBoxLayout>

ShapesTable::ShapesTable(QWidget* parent) : QDialog(parent) {
  setWindowTitle(tr("Таблиця об'єктів"));
  setModal(false);
  resize(480, 360);

  table = new QTableWidget(0, 4, this);
  table->setHorizontalHeaderLabels({ tr("No"), tr("Тип"), tr("Початок (x, y)"), tr("Кінець (x, y)") });
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->verticalHeader()->setVisible(false);
  table->horizontalHeader()->setStretchLastSection(true);
  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  deleteButton = new QPushButton(tr("Видалити виділений"), this);

  auto* buttons = new QHBoxLayout();
  buttons->addStretch();
  buttons->addWidget(deleteButton);

  auto* layout = new QVBoxLayout(this);
  layout->addWidget(table);
  layout->addLayout(buttons);

  auto* deleteShortcut = new QShortcut(QKeySequence::Delete, this);
  deleteShortcut->setContext(Qt::WidgetWithChildrenShortcut);

  connect(table, &QTableWidget::itemSelectionChanged, this, &ShapesTable::handleSelectionChanged);
  connect(deleteButton, &QPushButton::clicked, this, &ShapesTable::handleDeleteRequested);
  connect(deleteShortcut, &QShortcut::activated, this, &ShapesTable::handleDeleteRequested);
}

void ShapesTable::setRows(const std::vector<Row>& rows) {
  table->blockSignals(true);
  table->clearContents();
  table->setRowCount(static_cast<int>(rows.size()));

  for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
    const Row& row = rows[static_cast<size_t>(i)];
    table->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
    table->setItem(i, 1, new QTableWidgetItem(row.name));
    table->setItem(i, 2, new QTableWidgetItem(row.start));
    table->setItem(i, 3, new QTableWidgetItem(row.end));
  }

  table->clearSelection();
  table->blockSignals(false);
}

void ShapesTable::done(int result) {
  QDialog::done(result);
  if (onClosed) {
    onClosed();
  }
}

int ShapesTable::currentRow() const {
  const auto selected = table->selectionModel()->selectedRows();
  return selected.isEmpty() ? -1 : selected.first().row();
}

void ShapesTable::handleSelectionChanged() {
  if (onRowSelected) {
    onRowSelected(currentRow());
  }
}

void ShapesTable::handleDeleteRequested() {
  const int row = currentRow();
  if (row >= 0 && onRowDeleteRequested) {
    onRowDeleteRequested(row);
  }
}
