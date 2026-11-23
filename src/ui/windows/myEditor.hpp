#ifndef INCLUDE_UI_WINDOWS_MAIN_WINDOW_HPP_
#define INCLUDE_UI_WINDOWS_MAIN_WINDOW_HPP_

#include <QMainWindow>
#include <QtWidgets/QToolBar>
#include <cstdint>
#include <utility>
#include "../components/shapesCanvas.hpp"
#include "../popups/shapesTable.hpp"

class MyEditor : public QMainWindow {
  Q_OBJECT

public:
  static MyEditor* getInstance();
  static void destroyInstance();


  template<std::derived_from<Shape> T>
  void registerShape() {
    std::unique_ptr<Shape> object = std::make_unique<T>();

    QAction* action = new QAction(this);
    action->setIcon(object->getToolbarIcon());
    action->setToolTip(tr(object->getShapeName()));
    action->setCheckable(true);
    action->setData(object->getShapeID());
    toolbarShapesButtons.emplace(object->getShapeID(), action);
    shapesToolbar->addAction(action);

    registeredShapes.push_back(std::move(object));
    updateShapesMenu();
  }

private:
  explicit MyEditor(QWidget* parent = nullptr) noexcept;
  ~MyEditor() noexcept = default;
  QMenu* shapesMenu { nullptr };
  QToolBar* shapesToolbar { nullptr };
  ShapesCanvas* canvas { nullptr };
  ShapesTable* shapesTable { nullptr };
  QAction* tableAction { nullptr };
  uint64_t selectedShapeID { 0 };
  std::vector<std::unique_ptr<Shape>> registeredShapes;
  std::map<uint8_t, QAction*> toolbarShapesButtons;

  void initUI() noexcept;
  void initMenu() noexcept;
  void initTable();
  void updateShapesMenu() noexcept;
  void changeShape(uint8_t shapeID);
  void refreshTable();
  std::unique_ptr<Shape> createShapeByID(uint8_t shapeID) const;

signals:
private slots:
  void shapeSelected(QAction* action);
  void toggleTable(bool visible);
  void saveToFile();
  void loadFromFile();
};

#endif
