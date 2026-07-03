#include "mainwindow.h"
#include <QApplication>

static const char *kDarkStyleSheet = R"(
QWidget {
  background-color: #2b2d30;
  color: #d8d8d8;
  font-size: 15px;
}

QMainWindow, QMessageBox {
  background-color: #2b2d30;
}

QLabel, QCheckBox {
  background-color: transparent;
}

QGroupBox {
  background-color: #313338;
  border: 1px solid #3f4145;
  border-radius: 10px;
  margin-top: 14px;
  padding: 10px;
}

QTableWidget {
  background-color: #313338;
  alternate-background-color: #35373b;
  gridline-color: #3f4145;
  border: 1px solid #3f4145;
  border-radius: 10px;
  selection-background-color: #4a4d52;
  selection-color: #ffffff;
}

QTableWidget::item {
  padding: 4px;
  border: none;
}

QHeaderView::section {
  background-color: #3a3c40;
  color: #cfcfcf;
  padding: 6px;
  border: none;
  border-bottom: 1px solid #3f4145;
}

QHeaderView::section:first {
  border-top-left-radius: 10px;
}

QHeaderView::section:last {
  border-top-right-radius: 10px;
}

QLineEdit, QSpinBox {
  background-color: #3a3c40;
  border: 1px solid #46484c;
  border-radius: 8px;
  padding: 6px 8px;
  selection-background-color: #5a5d63;
}

QLineEdit:read-only {
  color: #9a9a9a;
}

QLineEdit:focus, QSpinBox:focus {
  border: 1px solid #6a6d73;
}

QPushButton {
  background-color: #45474c;
  border: 1px solid #4f5257;
  border-radius: 8px;
  padding: 7px 16px;
}

QPushButton:hover {
  background-color: #505257;
}

QPushButton:pressed {
  background-color: #3a3c40;
}

QCheckBox::indicator {
  width: 15px;
  height: 15px;
  border-radius: 4px;
  border: 1px solid #5a5c61;
  background-color: #3a3c40;
}

QCheckBox::indicator:checked {
  background-color: #7a7d84;
  border: 1px solid #9a9da3;
  image: url(:/icons/check.svg);
}

QLineEdit:disabled, QSpinBox:disabled {
  color: #6a6c70;
  background-color: #333538;
  border: 1px solid #3a3c40;
}

QSpinBox::up-button, QSpinBox::down-button {
  background-color: #45474c;
  border: none;
  width: 14px;
}

QScrollBar:vertical, QScrollBar:horizontal {
  background: #2b2d30;
  border: none;
  border-radius: 5px;
  width: 10px;
  height: 10px;
}

QScrollBar::handle {
  background: #4a4d52;
  border-radius: 5px;
  min-height: 20px;
}

QScrollBar::handle:hover {
  background: #5a5d63;
}

QScrollBar::add-line, QScrollBar::sub-line {
  height: 0;
  width: 0;
}
)";

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setStyleSheet(kDarkStyleSheet);

  MainWindow window;

  window.show();

  return app.exec();
}
