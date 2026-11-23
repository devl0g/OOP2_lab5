#ifndef INCLUDE_UI_POPUPS_SHAPES_TABLE_HPP_
#define INCLUDE_UI_POPUPS_SHAPES_TABLE_HPP_

#include <QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <functional>
#include <vector>

using RowCallback = std::function<void(int)>; // -1 means nothing selected
using VoidCallback = std::function<void()>;

class ShapesTable : public QDialog {
  Q_OBJECT

public:
  struct Row {
    QString name;
    QString start;
    QString end;
  };

  explicit ShapesTable(QWidget* parent = nullptr);
  ~ShapesTable() override = default;

  void setRows(const std::vector<Row>& rows);

  void setOnRowSelected(RowCallback callback) {
    this->onRowSelected = callback;
  }
  void setOnRowDeleteRequested(RowCallback callback) {
    this->onRowDeleteRequested = callback;
  }
  void setOnClosed(VoidCallback callback) {
    this->onClosed = callback;
  }

protected:
  void done(int result) override;

private slots:
  void handleSelectionChanged();
  void handleDeleteRequested();

private:
  int currentRow() const;

  QTableWidget* table { nullptr };
  QPushButton* deleteButton { nullptr };
  RowCallback onRowSelected;
  RowCallback onRowDeleteRequested;
  VoidCallback onClosed;
};

#endif
