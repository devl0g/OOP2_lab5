#include "myEditor.hpp"
#include "myEditor.hpp"
#include "../../config.hpp"
#include <QtWidgets/QMenuBar>
#include <iostream>
#include <qdir.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <string>

static MyEditor* instance = nullptr;

MyEditor* MyEditor::getInstance() {
  if (!instance) {
    instance = new MyEditor();
  }
  return instance;
}

void MyEditor::destroyInstance() {
  delete instance;
  instance = nullptr;
}


MyEditor::MyEditor(QWidget* parent) noexcept : 
  QMainWindow(parent) {

  initUI();
  initMenu();
}

// UI

void MyEditor::initUI() noexcept {
  setWindowTitle(tr(WINDOW_TITLE));

  canvas = new ShapesCanvas(this);
  setCentralWidget(canvas);

  connect(canvas, &ShapesCanvas::shapesChanged, this, &MyEditor::refreshTable);
}

void MyEditor::initMenu() noexcept {
  QMenu* fileMenu = menuBar()->addMenu(tr("&Файл"));

  QAction* openAction = fileMenu->addAction(tr("&Відкрити..."));
  openAction->setShortcut(QKeySequence::Open);
  connect(openAction, &QAction::triggered, this, &MyEditor::loadFromFile);

  QAction* saveAction = fileMenu->addAction(tr("&Зберегти як..."));
  saveAction->setShortcut(QKeySequence::Save);
  connect(saveAction, &QAction::triggered, this, &MyEditor::saveToFile);

  fileMenu->addSeparator();

  QAction* tableAction = fileMenu->addAction(tr("&Таблиця об'єктів"));
  tableAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));

  fileMenu->addSeparator();

  QAction* exitAction = fileMenu->addAction(tr("Ви&йти"));
  connect(exitAction, &QAction::triggered, this, [this] { close(); });

  shapesMenu = menuBar()->addMenu(tr("&Об'єкти"));
  menuBar()->addMenu(tr("&Довідка"));

  shapesToolbar = new QToolBar("Об'єкти", this);
  addToolBar(shapesToolbar);

  connect(tableAction, &QAction::triggered, this, &MyEditor::toggleTable);
  connect(shapesMenu, &QMenu::triggered, this, &MyEditor::shapeSelected);
  connect(shapesToolbar, &QToolBar::actionTriggered, this, &MyEditor::shapeSelected);
}

void MyEditor::refreshTable() {
  std::vector<ShapesTable::Row> rows;
  rows.reserve(canvas->getShapesCount());

  for (uint64_t i = 0; i < canvas->getShapesCount(); ++i) {
    const ShapeRecord& record = canvas->getRecord(i);
    rows.push_back({
      QString::fromUtf8(record.shape->getShapeName()),
      QString("(%1, %2)").arg(record.start.x).arg(record.start.y),
      QString("(%1, %2)").arg(record.end.x).arg(record.end.y),
    });
  }

  shapesTable->setRows(rows);
}

std::unique_ptr<Shape> MyEditor::createShapeByID(uint8_t shapeID) const {
  for (const auto& shape : registeredShapes) {
    if (shape->getShapeID() == shapeID) {
      return shape->createInstance();
    }
  }
  return nullptr;
}

void MyEditor::updateShapesMenu() noexcept {
  shapesMenu->clear();

  for (const auto& shape : registeredShapes) {
    auto action = shapesMenu->addAction(tr(shape->getShapeName()));
    action->setData(shape->getShapeID());
  }
}

void MyEditor::changeShape(uint8_t shapeID) {
  for (const auto& shape : registeredShapes) {
    if (shape->getShapeID() == shapeID) {
      selectedShapeID = shapeID;
      canvas->setDrawingShape(shape->createInstance());
      const auto newTitle = std::string("Обрано: ") + shape->getShapeName();
      setWindowTitle(tr(newTitle.c_str()));
    }
  }

  for (const auto& action : toolbarShapesButtons) {
    action.second->setChecked(false);
  }

  toolbarShapesButtons.at(shapeID)->setChecked(true);
}

// Slots

void MyEditor::shapeSelected(QAction* action) {
  const uint32_t data = action->data().toUInt();
  changeShape(data);
}

void MyEditor::toggleTable(bool visible) {
  shapesTable->show();
  shapesTable->raise();
  shapesTable->activateWindow();
}

void MyEditor::saveToFile() {
  QFileDialog dialog(this, "Save someting", QString(),
                   "Comma-separated file (*.csv)");
  dialog.setDefaultSuffix(".csv");
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (!dialog.exec()) return;
  const auto path = dialog.selectedFiles().front();

  if (path.isEmpty()) {
    return;
  }

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Помилка"), tr("Не вдалося відкрити файл для запису."));
    return;
  }

  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);
  out << "# Lab5 shapes: <ID> <x1> <y1> <x2> <y2>  # name\n";

  for (uint64_t i = 0; i < canvas->getShapesCount(); ++i) {
    const ShapeRecord& r = canvas->getRecord(i);
    out << static_cast<int>(r.shape->getShapeID()) << ' '
        << r.start.x << ' ' << r.start.y << ' '
        << r.end.x << ' ' << r.end.y
        << "  # " << QString::fromUtf8(r.shape->getShapeName()) << '\n';
  }
}

void MyEditor::loadFromFile() {
  const QString path = QFileDialog::getOpenFileName(
    this, tr("Відкрити об'єкти"), QString(), tr("CSV файли (*.csv);;Усі файли (*)")
  );

  if (path.isEmpty()) {
    return;
  }

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Помилка"), tr("Не вдалося відкрити файл для читання."));
    return;
  }

  struct Item { uint8_t id; Position start; Position end; };
  std::vector<Item> items;

  QTextStream in(&file);
  in.setEncoding(QStringConverter::Utf8);
  int lineNumber = 0;

  while (!in.atEnd()) {
    const QString line = in.readLine();
    ++lineNumber;

    const QString content = line.section('#', 0, 0).simplified();
    if (content.isEmpty()) {
      continue;
    }

    const QStringList parts = content.split(' ');
    bool ok[5] {};
    Item item {};
    bool valid = parts.size() == 5;

    if (valid) {
      const uint id = parts[0].toUInt(&ok[0]);
      item.id = static_cast<uint8_t>(id);
      item.start.x = parts[1].toLongLong(&ok[1]);
      item.start.y = parts[2].toLongLong(&ok[2]);
      item.end.x = parts[3].toLongLong(&ok[3]);
      item.end.y = parts[4].toLongLong(&ok[4]);
      valid = ok[0] && ok[1] && ok[2] && ok[3] && ok[4] && id <= 255;
    }

    if (valid) {
      valid = createShapeByID(item.id) != nullptr;
    }

    if (!valid) {
      QMessageBox::warning(
        this, tr("Помилка"), tr("Некоректний рядок %1 у файлі.").arg(lineNumber)
      );
      return;
    }

    items.push_back(item);
  }

  canvas->blockSignals(true);
  canvas->clearShapes();

  bool truncated = false;
  for (const Item& item : items) {
    auto shape = createShapeByID(item.id);
    shape->updateShapeConfig(item.start, item.end);
    if (!canvas->addShape(std::move(shape), item.start, item.end)) {
      truncated = true;
      break;
    }
  }

  canvas->blockSignals(false);
  refreshTable();
  canvas->update();

  if (truncated) {
    QMessageBox::information(
      this, tr("Увага"),
      tr("Файл містить більше об'єктів, ніж дозволено (%1). Зайві пропущено.").arg(MAX_CANVAS_SHAPES)
    );
  }
}
