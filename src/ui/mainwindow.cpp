#include "mainwindow.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("HyprMonitor - Rule Manager");
  resize(900, 700);

  setupUi();
  refreshWindowList();
}

void MainWindow::setupUi() {
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  QHBoxLayout *controlsLayout = new QHBoxLayout();

  QHBoxLayout *browseLayout = new QHBoxLayout();
  m_browseButton = new QPushButton("Browse monitors.lua", this);
  m_configFileLabel = new QLabel("No file selected", this);
  browseLayout->addWidget(m_browseButton);
  browseLayout->addWidget(m_configFileLabel, 1);

  m_rulesTableWidget = new QTableWidget(this);
  m_rulesTableWidget->setColumnCount(6);
  m_rulesTableWidget->setHorizontalHeaderLabels(
      {"Name", "Match Class", "Match Title", "Float", "Size", "Move"});
  m_rulesTableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  m_rulesTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
  m_rulesTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_rulesTableWidget->verticalHeader()->setVisible(false);

  m_tableWidget = new QTableWidget(this);
  m_tableWidget->setColumnCount(4);
  m_tableWidget->setHorizontalHeaderLabels(
      {"Class", "Title", "Workspace", "State"});
  m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_tableWidget->verticalHeader()->setVisible(false);

  m_formGroup = new QGroupBox("Rule", this);
  QVBoxLayout *formGroupLayout = new QVBoxLayout(m_formGroup);

  QHBoxLayout *formHeaderLayout = new QHBoxLayout();
  QPushButton *closeRuleButton = new QPushButton("Close", this);
  formHeaderLayout->addStretch();
  formHeaderLayout->addWidget(closeRuleButton);
  formGroupLayout->addLayout(formHeaderLayout);

  QFormLayout *formLayout = new QFormLayout();
  formGroupLayout->addLayout(formLayout);

  m_nameEdit = new QLineEdit(this);
  formLayout->addRow("Name:", m_nameEdit);

  m_matchClassEdit = new QLineEdit(this);
  m_matchClassEdit->setReadOnly(true);
  formLayout->addRow("Match Class:", m_matchClassEdit);

  QHBoxLayout *matchTitleLayout = new QHBoxLayout();
  m_matchTitleCheckBox = new QCheckBox(this);
  m_matchTitleEdit = new QLineEdit(this);
  m_matchTitleEdit->setReadOnly(true);
  m_matchTitleEdit->setEnabled(false);
  matchTitleLayout->addWidget(m_matchTitleCheckBox);
  matchTitleLayout->addWidget(m_matchTitleEdit);
  formLayout->addRow("Match Title:", matchTitleLayout);

  connect(m_matchTitleCheckBox, &QCheckBox::toggled, m_matchTitleEdit,
          &QLineEdit::setEnabled);

  QHBoxLayout *floatLayout = new QHBoxLayout();
  m_floatCheckBox = new QCheckBox(this);
  floatLayout->addWidget(m_floatCheckBox);
  floatLayout->addStretch();
  formLayout->addRow("Float:", floatLayout);

  QHBoxLayout *sizeLayout = new QHBoxLayout();
  m_sizeCheckBox = new QCheckBox(this);
  m_sizeWidthSpin = new QSpinBox(this);
  m_sizeWidthSpin->setRange(1, 10000);
  m_sizeWidthSpin->setEnabled(false);
  m_sizeHeightSpin = new QSpinBox(this);
  m_sizeHeightSpin->setRange(1, 10000);
  m_sizeHeightSpin->setEnabled(false);
  sizeLayout->addWidget(m_sizeCheckBox);
  sizeLayout->addWidget(m_sizeWidthSpin);
  sizeLayout->addWidget(new QLabel("x", this));
  sizeLayout->addWidget(m_sizeHeightSpin);
  formLayout->addRow("Size:", sizeLayout);

  connect(m_sizeCheckBox, &QCheckBox::toggled, m_sizeWidthSpin,
          &QSpinBox::setEnabled);
  connect(m_sizeCheckBox, &QCheckBox::toggled, m_sizeHeightSpin,
          &QSpinBox::setEnabled);
  m_floatCheckBox->setChecked(true);  // TEMP: for screenshot verification
  m_sizeCheckBox->setChecked(true);   // TEMP: for screenshot verification
  qDebug() << "size checked?" << m_sizeCheckBox->isChecked()
           << "width enabled?" << m_sizeWidthSpin->isEnabled();

  QHBoxLayout *moveLayout = new QHBoxLayout();
  m_moveCheckBox = new QCheckBox(this);
  m_moveXSpin = new QSpinBox(this);
  m_moveXSpin->setRange(-10000, 10000);
  m_moveXSpin->setEnabled(false);
  m_moveYSpin = new QSpinBox(this);
  m_moveYSpin->setRange(-10000, 10000);
  m_moveYSpin->setEnabled(false);
  moveLayout->addWidget(m_moveCheckBox);
  moveLayout->addWidget(m_moveXSpin);
  moveLayout->addWidget(new QLabel(",", this));
  moveLayout->addWidget(m_moveYSpin);
  formLayout->addRow("Move:", moveLayout);

  connect(m_moveCheckBox, &QCheckBox::toggled, m_moveXSpin,
          &QSpinBox::setEnabled);
  connect(m_moveCheckBox, &QCheckBox::toggled, m_moveYSpin,
          &QSpinBox::setEnabled);

  m_refreshButton = new QPushButton("Refresh List", this);
  m_applyButton = new QPushButton("Apply Rule", this);

  controlsLayout->addWidget(m_refreshButton);
  controlsLayout->addStretch();
  controlsLayout->addWidget(m_applyButton);

  m_formGroup->setVisible(true); // TEMP: for screenshot verification

  mainLayout->addLayout(browseLayout);
  mainLayout->addWidget(m_rulesTableWidget, 1);
  mainLayout->addWidget(m_tableWidget, 2);
  mainLayout->addWidget(m_formGroup);
  mainLayout->addLayout(controlsLayout);

  setCentralWidget(centralWidget);

  connect(m_refreshButton, &QPushButton::clicked, this,
          &MainWindow::refreshWindowList);
  connect(m_applyButton, &QPushButton::clicked, this,
          &MainWindow::applySelectedRule);
  connect(m_tableWidget, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::populateFormFromSelection);
  connect(m_browseButton, &QPushButton::clicked, this,
          &MainWindow::browseConfigFile);
  connect(closeRuleButton, &QPushButton::clicked, this, [this]() {
    m_formGroup->setVisible(false);
    m_tableWidget->clearSelection();
  });
}

void MainWindow::refreshWindowList() {
  m_tableWidget->setRowCount(0);

  QVector<HyprWindow> currentWindows = m_hyprClient.fetchActiveWindows();

  for (int i = 0; i < currentWindows.size(); ++i) {
    m_tableWidget->insertRow(i);

    QTableWidgetItem *classItem =
        new QTableWidgetItem(currentWindows[i].wmClass);
    classItem->setData(Qt::UserRole, currentWindows[i].address);
    m_tableWidget->setItem(i, 0, classItem);
    m_tableWidget->setItem(i, 1, new QTableWidgetItem(currentWindows[i].title));
    m_tableWidget->setItem(
        i, 2,
        new QTableWidgetItem(QString::number(currentWindows[i].workspaceId)));

    QString stateStr = currentWindows[i].isFloating ? "Floating" : "Tiled";
    m_tableWidget->setItem(i, 3, new QTableWidgetItem(stateStr));
  }
}

void MainWindow::populateFormFromSelection() {
  int selectedRow = m_tableWidget->currentRow();
  if (selectedRow < 0)
    return;

  QTableWidgetItem *classItem = m_tableWidget->item(selectedRow, 0);
  QString wmClass = classItem->text();
  QString title = m_tableWidget->item(selectedRow, 1)->text();

  m_formGroup->setVisible(true);
  m_nameEdit->setText(wmClass.toLower() + "_rule");
  m_matchClassEdit->setText(wmClass + "$");
  m_matchTitleEdit->setText(title.isEmpty() ? "" : title + "$");
  m_matchTitleCheckBox->setChecked(!title.isEmpty());

  QString address = classItem->data(Qt::UserRole).toString();
  m_hyprClient.focusWindow(address);
}

void MainWindow::applySelectedRule() {
  if (m_nameEdit->text().isEmpty() || m_matchClassEdit->text().isEmpty()) {
    QMessageBox::warning(this, "Missing Fields",
                         "Please pick a window and provide at least a name "
                         "and a class to match.");
    return;
  }

  WindowRule rule;
  rule.name = m_nameEdit->text();
  rule.matchClass = m_matchClassEdit->text();
  rule.matchTitle =
      m_matchTitleCheckBox->isChecked() ? m_matchTitleEdit->text() : "";

  rule.floatEnabled = m_floatCheckBox->isChecked();

  rule.sizeEnabled = m_sizeCheckBox->isChecked();
  rule.sizeWidth = m_sizeWidthSpin->value();
  rule.sizeHeight = m_sizeHeightSpin->value();

  rule.moveEnabled = m_moveCheckBox->isChecked();
  rule.moveX = m_moveXSpin->value();
  rule.moveY = m_moveYSpin->value();

  if (m_hyprClient.appendWindowRule(rule)) {
    QMessageBox::information(
        this, "Success",
        QString("Successfully applied rule: '%1'").arg(rule.name));
  } else {
    QMessageBox::critical(this, "Error",
                          "Failed to update configuration file.");
  }
}

void MainWindow::browseConfigFile() {
  QString startDir = QDir::homePath() + "/.config/hypr";

  QString defaultPath;
  QDirIterator it(startDir, QStringList() << "monitors.lua", QDir::Files,
                   QDirIterator::Subdirectories);
  if (it.hasNext())
    defaultPath = it.next();

  QString path = QFileDialog::getOpenFileName(
      this, "Select monitors.lua",
      defaultPath.isEmpty() ? startDir : defaultPath, "Lua Files (*.lua)");

  if (path.isEmpty())
    return;

  loadRulesFromFile(path);
}

void MainWindow::loadRulesFromFile(const QString &path) {
  m_configFileLabel->setText(path);

  QVector<ExistingRule> rules = m_hyprClient.parseRulesFile(path);

  m_rulesTableWidget->setRowCount(0);
  for (int i = 0; i < rules.size(); ++i) {
    m_rulesTableWidget->insertRow(i);
    m_rulesTableWidget->setItem(i, 0, new QTableWidgetItem(rules[i].name));
    m_rulesTableWidget->setItem(i, 1,
                                new QTableWidgetItem(rules[i].matchClass));
    m_rulesTableWidget->setItem(i, 2,
                                new QTableWidgetItem(rules[i].matchTitle));
    m_rulesTableWidget->setItem(
        i, 3, new QTableWidgetItem(rules[i].floatEnabled ? "Yes" : "No"));
    m_rulesTableWidget->setItem(i, 4, new QTableWidgetItem(rules[i].size));
    m_rulesTableWidget->setItem(i, 5, new QTableWidgetItem(rules[i].move));
  }
}
