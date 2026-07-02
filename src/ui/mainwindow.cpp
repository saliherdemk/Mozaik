#include "mainwindow.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("HyprMonitor - Rule Manager");
  resize(900, 700);

  setupUi();
  refreshWindowList();

  QString defaultPath =
      QDir::homePath() + "/.config/hypr/modules/windowrules.lua";
  if (QFile::exists(defaultPath))
    loadRulesFromFile(defaultPath);
}

void MainWindow::setupUi() {
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  QHBoxLayout *controlsLayout = new QHBoxLayout();

  QHBoxLayout *browseLayout = new QHBoxLayout();
  m_browseButton = new QPushButton("Browse windowrules.lua", this);
  m_configFileLabel = new QLabel("No file selected", this);
  browseLayout->addWidget(m_browseButton);
  browseLayout->addWidget(m_configFileLabel, 1);

  m_rulesTableWidget = new QTableWidget(this);
  m_rulesTableWidget->setColumnCount(7);
  m_rulesTableWidget->setHorizontalHeaderLabels(
      {"Name", "Match Class", "Match Title", "Float", "Size", "Move", ""});
  m_rulesTableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  m_rulesTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_rulesTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  m_rulesTableWidget->verticalHeader()->setDefaultSectionSize(44);
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
  QPushButton *restoreButton = new QPushButton("Restore Old", this);
  m_applyButton = new QPushButton("Apply Rule", this);

  controlsLayout->addWidget(m_refreshButton);
  controlsLayout->addWidget(restoreButton);
  controlsLayout->addStretch();
  controlsLayout->addWidget(m_applyButton);

  m_formGroup->setVisible(false);

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
  connect(m_rulesTableWidget, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::populateFormFromRuleSelection);
  connect(m_browseButton, &QPushButton::clicked, this,
          &MainWindow::browseConfigFile);
  connect(restoreButton, &QPushButton::clicked, this,
          &MainWindow::restoreOldConfig);
  connect(closeRuleButton, &QPushButton::clicked, this,
          &MainWindow::closeRuleForm);
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
  if (selectedRow < 0 || m_tableWidget->selectedItems().isEmpty())
    return;

  m_editingRuleIndex = -1;
  m_rulesTableWidget->clearSelection();
  m_applyButton->setText("Apply Rule");

  QTableWidgetItem *classItem = m_tableWidget->item(selectedRow, 0);
  QString wmClass = classItem->text();
  QString title = m_tableWidget->item(selectedRow, 1)->text();

  m_formGroup->setVisible(true);
  m_nameEdit->setText(wmClass.toLower() + "_rule");
  m_matchClassEdit->setText("\"" + wmClass + "$\"");
  m_matchTitleEdit->setText(title.isEmpty() ? "" : "\"" + title + "$\"");
  m_matchTitleCheckBox->setChecked(!title.isEmpty());

  QString address = classItem->data(Qt::UserRole).toString();
  m_hyprClient.focusWindow(address);
}

void MainWindow::populateFormFromRuleSelection() {
  int row = m_rulesTableWidget->currentRow();
  if (row < 0 || row >= m_loadedRules.size() ||
      m_rulesTableWidget->selectedItems().isEmpty())
    return;

  const ExistingRule &rule = m_loadedRules[row];
  m_editingRuleIndex = row;
  m_tableWidget->clearSelection();
  m_applyButton->setText("Update Rule");

  m_formGroup->setVisible(true);
  m_nameEdit->setText(rule.name);
  m_matchClassEdit->setText(rule.matchClass);
  m_matchTitleEdit->setText(rule.matchTitle);
  m_matchTitleCheckBox->setChecked(!rule.matchTitle.isEmpty());
  m_floatCheckBox->setChecked(rule.floatEnabled);

  QStringList size = rule.size.split(' ', Qt::SkipEmptyParts);
  m_sizeCheckBox->setChecked(size.size() == 2);
  if (size.size() == 2) {
    m_sizeWidthSpin->setValue(size[0].toInt());
    m_sizeHeightSpin->setValue(size[1].toInt());
  }

  QStringList move = rule.move.split(' ', Qt::SkipEmptyParts);
  m_moveCheckBox->setChecked(move.size() == 2);
  if (move.size() == 2) {
    m_moveXSpin->setValue(move[0].toInt());
    m_moveYSpin->setValue(move[1].toInt());
  }
}

void MainWindow::applySelectedRule() {
  // Only commit the form when it's open; a hidden form means "just rewrite
  // the file from the current list" (e.g. after removals).
  if (m_formGroup->isVisible()) {
    if (m_nameEdit->text().isEmpty() || m_matchClassEdit->text().isEmpty()) {
      QMessageBox::warning(this, "Missing Fields",
                           "Please pick a window and provide at least a name "
                           "and a class to match.");
      return;
    }

    ExistingRule rule;
    rule.name = m_nameEdit->text();
    rule.matchClass = m_matchClassEdit->text();
    rule.matchTitle =
        m_matchTitleCheckBox->isChecked() ? m_matchTitleEdit->text() : "";
    rule.floatEnabled = m_floatCheckBox->isChecked();
    rule.size = m_sizeCheckBox->isChecked()
                    ? QString("%1 %2")
                          .arg(m_sizeWidthSpin->value())
                          .arg(m_sizeHeightSpin->value())
                    : "";
    rule.move = m_moveCheckBox->isChecked()
                    ? QString("%1 %2")
                          .arg(m_moveXSpin->value())
                          .arg(m_moveYSpin->value())
                    : "";

    if (m_editingRuleIndex >= 0)
      m_loadedRules[m_editingRuleIndex] = rule;
    else
      m_loadedRules.append(rule);
  }

  QString path = m_configFilePath.isEmpty()
                     ? QDir::homePath() + "/.config/hypr/modules/windowrules.lua"
                     : m_configFilePath;
  QFileInfo fi(path);
  QString oldPath = fi.path() + "/" + fi.completeBaseName() + "_old.lua";

  // ponytail: one-level undo — each apply replaces the previous backup
  if (QFile::exists(path)) {
    QFile::remove(oldPath);
    if (!QFile::rename(path, oldPath)) {
      QMessageBox::critical(this, "Error",
                            "Failed to back up current file to " + oldPath);
      return;
    }
  }

  if (m_hyprClient.writeRulesFile(path, m_loadedRules, m_configHeader)) {
    int count = m_loadedRules.size();
    loadRulesFromFile(path);
    closeRuleForm();
    QMessageBox::information(this, "Success",
                             QString("Saved %1 rules to %2\nBackup: %3")
                                 .arg(count)
                                 .arg(path, oldPath));
  } else {
    QMessageBox::critical(this, "Error",
                          "Failed to write configuration file.");
  }
}

void MainWindow::restoreOldConfig() {
  QString path = m_configFilePath.isEmpty()
                     ? QDir::homePath() + "/.config/hypr/modules/windowrules.lua"
                     : m_configFilePath;
  QFileInfo fi(path);
  QString oldPath = fi.path() + "/" + fi.completeBaseName() + "_old.lua";

  if (!QFile::exists(oldPath)) {
    QMessageBox::warning(this, "No Backup", "No backup file found:\n" + oldPath);
    return;
  }

  QFile::remove(path); // drop the updated version
  if (!QFile::rename(oldPath, path)) {
    QMessageBox::critical(this, "Error", "Failed to restore " + oldPath);
    return;
  }

  QProcess::startDetached("hyprctl", QStringList() << "reload");
  closeRuleForm();
  loadRulesFromFile(path);
  QMessageBox::information(this, "Restored",
                           "Restored backup to " + path);
}

void MainWindow::closeRuleForm() {
  m_formGroup->setVisible(false);
  m_tableWidget->clearSelection();
  m_rulesTableWidget->clearSelection();
  m_editingRuleIndex = -1;
  m_applyButton->setText("Apply Rule");
}

void MainWindow::browseConfigFile() {
  QString startDir = QDir::homePath() + "/.config/hypr";
  QString defaultPath = startDir + "/modules/windowrules.lua";

  QString path = QFileDialog::getOpenFileName(
      this, "Select windowrules.lua",
      QFile::exists(defaultPath) ? defaultPath : startDir,
      "Lua Files (*.lua)");

  if (path.isEmpty())
    return;

  loadRulesFromFile(path);
}

void MainWindow::loadRulesFromFile(const QString &path) {
  m_configFilePath = path;
  m_configFileLabel->setText(path);
  m_loadedRules = m_hyprClient.parseRulesFile(path, &m_configHeader);
  refreshRulesTable();
}

void MainWindow::refreshRulesTable() {
  m_rulesTableWidget->setRowCount(0);
  for (int i = 0; i < m_loadedRules.size(); ++i) {
    const ExistingRule &rule = m_loadedRules[i];
    m_rulesTableWidget->insertRow(i);
    m_rulesTableWidget->setItem(i, 0, new QTableWidgetItem(rule.name));
    m_rulesTableWidget->setItem(i, 1, new QTableWidgetItem(rule.matchClass));
    m_rulesTableWidget->setItem(i, 2, new QTableWidgetItem(rule.matchTitle));
    m_rulesTableWidget->setItem(
        i, 3, new QTableWidgetItem(rule.floatEnabled ? "Yes" : "No"));
    m_rulesTableWidget->setItem(i, 4, new QTableWidgetItem(rule.size));
    m_rulesTableWidget->setItem(i, 5, new QTableWidgetItem(rule.move));

    QPushButton *removeButton = new QPushButton("Remove", this);
    removeButton->setStyleSheet(
        "QPushButton { background-color: #c0392b; color: white; border: none;"
        " border-radius: 4px; margin: 6px; min-height: 24px; padding: 2px; }");
    connect(removeButton, &QPushButton::clicked, this, [this, i]() {
      m_loadedRules.removeAt(i);
      closeRuleForm();
      refreshRulesTable();
    });
    m_rulesTableWidget->setCellWidget(i, 6, removeButton);
  }
}
