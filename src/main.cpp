#include "mainwindow.h"
#include <QApplication>

static const char *kDarkStyleSheet = R"(
QWidget {
  background-color: #17191c;
  color: #e2e4e8;
  font-size: 15px;
}

QLabel, QCheckBox {
  background-color: transparent;
}

QLabel {
  color: #c2c7ce;
}

QGroupBox {
  background-color: #1f2226;
  border: 1px solid #3d4249;
  border-radius: 12px;
  margin-top: 8px;
  padding: 16px;
}

QTabWidget::pane {
  border: none;
  top: 8px;
}

QTabBar::tab {
  background: transparent;
  color: #c2c7ce;
  padding: 10px 24px;
  margin-right: 4px;
  border: none;
  border-bottom: 2px solid transparent;
}

QTabBar::tab:hover {
  color: #e2e4e8;
}

QTabBar::tab:selected {
  color: #ffffff;
  border-bottom: 2px solid #5c87b2;
}

QTableWidget {
  background-color: #1f2226;
  alternate-background-color: #24272c;
  gridline-color: #2a2d32;
  border: 1px solid #3d4249;
  border-radius: 12px;
  padding: 4px;
  selection-background-color: rgba(92, 135, 178, 0.30);
  selection-color: #ffffff;
}

QTableWidget::item {
  padding: 8px;
  border: none;
}

QHeaderView::section {
  background-color: #2a2d32;
  color: #f0f2f5;
  padding: 10px 8px;
  border: none;
  border-bottom: 1px solid #3d4249;
  font-weight: 600;
}

QHeaderView::section:first {
  border-top-left-radius: 12px;
}

QHeaderView::section:last {
  border-top-right-radius: 12px;
}

QLineEdit, QAbstractSpinBox {
  background-color: #26292e;
  border: 1px solid #3d4249;
  border-radius: 8px;
  padding: 8px 12px;
  selection-background-color: rgba(92, 135, 178, 0.45);
}

QLineEdit:read-only {
  color: #9aa0a8;
}

QLineEdit:focus, QAbstractSpinBox:focus {
  border: 1px solid #5c87b2;
  background-color: #2a2d32;
}

QPushButton {
  background-color: #26292e;
  border: 1px solid #454a51;
  border-radius: 8px;
  padding: 9px 20px;
}

QPushButton:hover {
  background-color: #2e3238;
  border: 1px solid #454a51;
}

QPushButton:pressed {
  background-color: #1f2226;
}

QPushButton#primary {
  background-color: #3d6288;
  border: 1px solid #4a76a3;
  color: #ffffff;
  font-weight: 600;
}

QPushButton#primary:hover {
  background-color: #47709b;
}

QPushButton#primary:pressed {
  background-color: #355475;
}

QCheckBox::indicator {
  width: 15px;
  height: 15px;
  border-radius: 4px;
  border: 1px solid #454a51;
  background-color: #26292e;
}

QCheckBox::indicator:hover {
  border: 1px solid #5c87b2;
}

QCheckBox::indicator:checked {
  background-color: #5c87b2;
  border: 1px solid #6d99c4;
  image: url(:/icons/check.svg);
}

QLineEdit:disabled, QAbstractSpinBox:disabled {
  color: #5c6068;
  background-color: #1d2023;
  border: 1px solid #26292e;
}

QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {
  width: 0;
  border: none;
}

QScrollBar:vertical, QScrollBar:horizontal {
  background: transparent;
  border: none;
  width: 10px;
  height: 10px;
}

QScrollBar::handle {
  background: #383c42;
  border-radius: 5px;
  min-height: 20px;
}

QScrollBar::handle:hover {
  background: #454a51;
}

QScrollBar::add-line, QScrollBar::sub-line {
  height: 0;
  width: 0;
}

QMessageBox QLabel {
  color: #e2e4e8;
}
)";

int main(int argc, char *argv[]) {

  qputenv("QT_QPA_PLATFORMTHEME", "xdgdesktopportal");

  QApplication app(argc, argv);
  app.setStyleSheet(kDarkStyleSheet);

  MainWindow window;

  window.show();

  return app.exec();
}
