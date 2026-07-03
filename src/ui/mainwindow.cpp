#include "mainwindow.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("HyprMonitor - Rule Manager");
  resize(900, 700);

  setupUi();
  refreshWindowList();

  m_configFilePath =
      QSettings("HyprMonitor", "HyprMonitor").value("configFile").toString();
  if (QFile::exists(m_configFilePath))
    loadRulesFromFile(m_configFilePath);
  else
    browseConfigFile(); 

  if (m_configFilePath.isEmpty()) 
    m_configFilePath =
        QDir::homePath() + "/.config/hypr/modules/windowrules.lua";
}

void MainWindow::positionRuleForm() {
  if (!m_formGroup->isVisible())
    return;
  QWidget *cw = centralWidget();
  const int margin = 24;
  int w = cw->width() - 2 * margin;
  int h = m_formGroup->sizeHint().height();
  int y = m_controlsLayout->geometry().top() - h - 8;
  m_formGroup->setGeometry(margin, y, w, h);
  m_closeRuleButton->move(w - 46, 13);
  m_formGroup->raise();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  positionRuleForm();
}

QString MainWindow::backupPath() const {
  QFileInfo fi(m_configFilePath);
  return fi.path() + "/" + fi.completeBaseName() + "_old.lua";
}

void MainWindow::setupUi() {
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setSpacing(12);
  QHBoxLayout *controlsLayout = new QHBoxLayout();

  QHBoxLayout *browseLayout = new QHBoxLayout();
  QPushButton *browseButton = new QPushButton("Browse windowrules.lua", this);
  m_configFileLabel = new QLabel("No file selected", this);
  browseLayout->addWidget(browseButton);
  browseLayout->addWidget(m_configFileLabel, 1);

  m_rulesTableWidget = new QTableWidget(this);
  m_rulesTableWidget->setColumnCount(8);
  m_rulesTableWidget->setHorizontalHeaderLabels({"Name", "Match Class",
                                                 "Match Title", "Float", "Size",
                                                 "Move", "Opacity", ""});
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

  m_formGroup = new QGroupBox(centralWidget);
  QVBoxLayout *formGroupLayout = new QVBoxLayout(m_formGroup);

  m_closeRuleButton = new QPushButton("✕", m_formGroup);
  m_closeRuleButton->setFixedSize(30, 30);
  m_closeRuleButton->setStyleSheet(
      "QPushButton { padding: 0; border: none; border-radius: 6px;"
      " color: #c2c7ce; background: transparent; }"
      "QPushButton:hover { background-color: #383c42; color: #ffffff; }");

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setContentsMargins(0, 12, 28, 0);
  formGroupLayout->addLayout(formLayout);

  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(m_formGroup);
  shadow->setBlurRadius(32);
  shadow->setOffset(0, 8);
  shadow->setColor(QColor(0, 0, 0, 160));
  m_formGroup->setGraphicsEffect(shadow);

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

  QGridLayout *attrGrid = new QGridLayout();
  attrGrid->setContentsMargins(0, 8, 28, 0);
  attrGrid->setHorizontalSpacing(32);
  attrGrid->setVerticalSpacing(10);
  attrGrid->setColumnStretch(0, 1);
  attrGrid->setColumnStretch(1, 1);
  formGroupLayout->addLayout(attrGrid);

  QHBoxLayout *floatLayout = new QHBoxLayout();
  m_floatCheckBox = new QCheckBox("Float", this);
  floatLayout->addWidget(m_floatCheckBox);
  floatLayout->addStretch();
  attrGrid->addLayout(floatLayout, 0, 0);

  QHBoxLayout *sizeLayout = new QHBoxLayout();
  m_sizeCheckBox = new QCheckBox("Size", this);
  m_sizeWidthSpin = new QSpinBox(this);
  m_sizeWidthSpin->setRange(1, 10000);
  m_sizeWidthSpin->setEnabled(false);
  m_sizeHeightSpin = new QSpinBox(this);
  m_sizeHeightSpin->setRange(1, 10000);
  m_sizeHeightSpin->setEnabled(false);
  sizeLayout->setSpacing(8);
  sizeLayout->addWidget(m_sizeCheckBox);
  sizeLayout->addSpacing(12);
  sizeLayout->addWidget(m_sizeWidthSpin);
  sizeLayout->addWidget(new QLabel("x", this));
  sizeLayout->addWidget(m_sizeHeightSpin);
  sizeLayout->addStretch();
  attrGrid->addLayout(sizeLayout, 0, 1);

  connect(m_sizeCheckBox, &QCheckBox::toggled, m_sizeWidthSpin,
          &QSpinBox::setEnabled);
  connect(m_sizeCheckBox, &QCheckBox::toggled, m_sizeHeightSpin,
          &QSpinBox::setEnabled);

  QHBoxLayout *moveLayout = new QHBoxLayout();
  m_moveCheckBox = new QCheckBox("Move", this);
  m_moveXSpin = new QSpinBox(this);
  m_moveXSpin->setRange(-10000, 10000);
  m_moveXSpin->setEnabled(false);
  m_moveYSpin = new QSpinBox(this);
  m_moveYSpin->setRange(-10000, 10000);
  m_moveYSpin->setEnabled(false);
  moveLayout->setSpacing(8);
  moveLayout->addWidget(m_moveCheckBox);
  moveLayout->addSpacing(12);
  moveLayout->addWidget(m_moveXSpin);
  moveLayout->addWidget(new QLabel(",", this));
  moveLayout->addWidget(m_moveYSpin);
  moveLayout->addStretch();
  attrGrid->addLayout(moveLayout, 1, 0);

  connect(m_moveCheckBox, &QCheckBox::toggled, m_moveXSpin,
          &QSpinBox::setEnabled);
  connect(m_moveCheckBox, &QCheckBox::toggled, m_moveYSpin,
          &QSpinBox::setEnabled);

  QHBoxLayout *opacityLayout = new QHBoxLayout();
  m_opacityCheckBox = new QCheckBox("Opacity", this);
  m_opacityActiveSpin = new QDoubleSpinBox(this);
  m_opacityActiveSpin->setRange(0.0, 1.0);
  m_opacityActiveSpin->setSingleStep(0.05);
  m_opacityActiveSpin->setValue(1.0);
  m_opacityActiveSpin->setEnabled(false);
  m_opacityInactiveSpin = new QDoubleSpinBox(this);
  m_opacityInactiveSpin->setRange(0.0, 1.0);
  m_opacityInactiveSpin->setSingleStep(0.05);
  m_opacityInactiveSpin->setValue(0.9);
  m_opacityInactiveSpin->setEnabled(false);
  m_opacityActiveSpin->setToolTip("Active opacity");
  m_opacityInactiveSpin->setToolTip("Inactive opacity");
  opacityLayout->setSpacing(8);
  opacityLayout->addWidget(m_opacityCheckBox);
  opacityLayout->addSpacing(12);
  opacityLayout->addWidget(m_opacityActiveSpin);
  opacityLayout->addWidget(new QLabel(",", this));
  opacityLayout->addWidget(m_opacityInactiveSpin);
  opacityLayout->addStretch();
  attrGrid->addLayout(opacityLayout, 1, 1);

  connect(m_opacityCheckBox, &QCheckBox::toggled, m_opacityActiveSpin,
          &QDoubleSpinBox::setEnabled);
  connect(m_opacityCheckBox, &QCheckBox::toggled, m_opacityInactiveSpin,
          &QDoubleSpinBox::setEnabled);

  QPushButton *refreshButton = new QPushButton("Refresh List", this);
  QPushButton *restoreButton = new QPushButton("Restore Old", this);
  QPushButton *applyButton = new QPushButton("Apply Rule", this);
  applyButton->setObjectName("primary");

  controlsLayout->addWidget(refreshButton);
  controlsLayout->addWidget(restoreButton);
  controlsLayout->addStretch();
  controlsLayout->addWidget(applyButton);
  restoreButton->setVisible(false);

  m_formGroup->setVisible(false);

  QTabWidget *tabs = new QTabWidget(this);
  tabs->addTab(m_tableWidget, "Active Windows");

  QWidget *savedTab = new QWidget(this);
  QVBoxLayout *savedLayout = new QVBoxLayout(savedTab);
  savedLayout->addLayout(browseLayout);
  savedLayout->addWidget(m_rulesTableWidget);
  tabs->addTab(savedTab, "Saved Rules");

  mainLayout->addWidget(tabs, 1);
  mainLayout->addLayout(controlsLayout);
  m_controlsLayout = controlsLayout;

  setCentralWidget(centralWidget);

  connect(refreshButton, &QPushButton::clicked, this,
          &MainWindow::refreshWindowList);

  connect(tabs, &QTabWidget::currentChanged, this,
          [refreshButton, restoreButton](int index) {
            refreshButton->setVisible(index == 0);
            restoreButton->setVisible(index == 1);
          });
  connect(applyButton, &QPushButton::clicked, this,
          &MainWindow::applySelectedRule);
  connect(m_tableWidget, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::populateFormFromSelection);
  connect(m_rulesTableWidget, &QTableWidget::itemSelectionChanged, this,
          &MainWindow::populateFormFromRuleSelection);
  connect(browseButton, &QPushButton::clicked, this,
          &MainWindow::browseConfigFile);
  connect(restoreButton, &QPushButton::clicked, this,
          &MainWindow::restoreOldConfig);
  connect(m_closeRuleButton, &QPushButton::clicked, this,
          &MainWindow::closeRuleForm);

  connect(m_nameEdit, &QLineEdit::textChanged, this,
          &MainWindow::syncFormToRule);
  connect(m_matchTitleCheckBox, &QCheckBox::toggled, this,
          &MainWindow::syncFormToRule);
  connect(m_floatCheckBox, &QCheckBox::toggled, this,
          &MainWindow::syncFormToRule);
  connect(m_sizeCheckBox, &QCheckBox::toggled, this,
          &MainWindow::syncFormToRule);
  connect(m_moveCheckBox, &QCheckBox::toggled, this,
          &MainWindow::syncFormToRule);
  connect(m_sizeWidthSpin, qOverload<int>(&QSpinBox::valueChanged), this,
          &MainWindow::syncFormToRule);
  connect(m_sizeHeightSpin, qOverload<int>(&QSpinBox::valueChanged), this,
          &MainWindow::syncFormToRule);
  connect(m_moveXSpin, qOverload<int>(&QSpinBox::valueChanged), this,
          &MainWindow::syncFormToRule);
  connect(m_moveYSpin, qOverload<int>(&QSpinBox::valueChanged), this,
          &MainWindow::syncFormToRule);
  connect(m_opacityCheckBox, &QCheckBox::toggled, this,
          &MainWindow::syncFormToRule);
  connect(m_opacityActiveSpin, qOverload<double>(&QDoubleSpinBox::valueChanged),
          this, &MainWindow::syncFormToRule);
  connect(m_opacityInactiveSpin,
          qOverload<double>(&QDoubleSpinBox::valueChanged), this,
          &MainWindow::syncFormToRule);
}

ExistingRule MainWindow::ruleFromForm() const {
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
  rule.opacity = m_opacityCheckBox->isChecked()
                     ? QString("%1 %2")
                           .arg(m_opacityActiveSpin->value())
                           .arg(m_opacityInactiveSpin->value())
                     : "";
  return rule;
}

void MainWindow::syncFormToRule() {
  if (m_populatingForm || m_editingRuleIndex < 0)
    return;

  const ExistingRule rule = ruleFromForm();
  m_loadedRules[m_editingRuleIndex] = rule;

  int i = m_editingRuleIndex;
  m_rulesTableWidget->item(i, 0)->setText(rule.name);
  m_rulesTableWidget->item(i, 1)->setText(rule.matchClass);
  m_rulesTableWidget->item(i, 2)->setText(rule.matchTitle);
  m_rulesTableWidget->item(i, 3)->setText(rule.floatEnabled ? "Yes" : "No");
  m_rulesTableWidget->item(i, 4)->setText(rule.size);
  m_rulesTableWidget->item(i, 5)->setText(rule.move);
  m_rulesTableWidget->item(i, 6)->setText(rule.opacity);
}

void MainWindow::refreshWindowList() {
  m_tableWidget->setRowCount(0);

  QVector<HyprWindow> currentWindows = HyprClient::fetchActiveWindows();

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

  QTableWidgetItem *classItem = m_tableWidget->item(selectedRow, 0);
  QString wmClass = classItem->text();
  QString title = m_tableWidget->item(selectedRow, 1)->text();

  m_populatingForm = true;
  m_formGroup->setVisible(true);
  m_nameEdit->setText(wmClass.toLower() + "_rule");
  m_matchClassEdit->setText("\"" + wmClass + "$\"");
  m_matchTitleEdit->setText(title.isEmpty() ? "" : "\"" + title + "$\"");
  m_matchTitleCheckBox->setChecked(!title.isEmpty());
  m_floatCheckBox->setChecked(false);
  m_sizeCheckBox->setChecked(false);
  m_sizeWidthSpin->setValue(800);
  m_sizeHeightSpin->setValue(600);
  m_moveCheckBox->setChecked(false);
  m_moveXSpin->setValue(0);
  m_moveYSpin->setValue(0);
  m_opacityCheckBox->setChecked(false);
  m_opacityActiveSpin->setValue(1.0);
  m_opacityInactiveSpin->setValue(0.9);
  m_populatingForm = false;
  positionRuleForm();

  HyprClient::focusWindow(classItem->data(Qt::UserRole).toString());
}

void MainWindow::populateFormFromRuleSelection() {
  int row = m_rulesTableWidget->currentRow();
  if (row < 0 || row >= m_loadedRules.size() ||
      m_rulesTableWidget->selectedItems().isEmpty())
    return;

  const ExistingRule &rule = m_loadedRules[row];
  m_editingRuleIndex = row;
  m_tableWidget->clearSelection();

  m_populatingForm = true;
  m_formGroup->setVisible(true);
  m_nameEdit->setText(rule.name);
  m_matchClassEdit->setText(rule.matchClass);
  m_matchTitleEdit->setText(rule.matchTitle);
  m_matchTitleCheckBox->setChecked(!rule.matchTitle.isEmpty());
  m_floatCheckBox->setChecked(rule.floatEnabled);

  QStringList size = rule.size.split(' ', Qt::SkipEmptyParts);
  m_sizeCheckBox->setChecked(size.size() == 2);
  m_sizeWidthSpin->setValue(size.size() == 2 ? size[0].toInt() : 800);
  m_sizeHeightSpin->setValue(size.size() == 2 ? size[1].toInt() : 600);

  QStringList move = rule.move.split(' ', Qt::SkipEmptyParts);
  m_moveCheckBox->setChecked(move.size() == 2);
  m_moveXSpin->setValue(move.size() == 2 ? move[0].toInt() : 0);
  m_moveYSpin->setValue(move.size() == 2 ? move[1].toInt() : 0);

  QStringList opacity = rule.opacity.split(' ', Qt::SkipEmptyParts);
  opacity.removeAll("override");
  m_opacityCheckBox->setChecked(!opacity.isEmpty());
  m_opacityActiveSpin->setValue(opacity.isEmpty() ? 1.0
                                                  : opacity[0].toDouble());
  m_opacityInactiveSpin->setValue(opacity.size() > 1 ? opacity[1].toDouble()
                                  : opacity.isEmpty() ? 0.9
                                                      : opacity[0].toDouble());
  m_populatingForm = false;
  positionRuleForm();
}

void MainWindow::applySelectedRule() {
  if (m_formGroup->isVisible()) {
    if (m_nameEdit->text().isEmpty() || m_matchClassEdit->text().isEmpty()) {
      QMessageBox::warning(this, "Missing Fields",
                           "Please pick a window and provide at least a name "
                           "and a class to match.");
      return;
    }

    ExistingRule rule = ruleFromForm();
    if (m_editingRuleIndex >= 0)
      m_loadedRules[m_editingRuleIndex] = rule;
    else
      m_loadedRules.append(rule);
  }

  const QString path = m_configFilePath;
  const QString oldPath = backupPath();

  if (QFile::exists(path)) {
    QFile::remove(oldPath);
    if (!QFile::rename(path, oldPath)) {
      QMessageBox::critical(this, "Error",
                            "Failed to back up current file to " + oldPath);
      return;
    }
  }

  if (HyprClient::writeRulesFile(path, m_loadedRules, m_configHeader)) {
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
  const QString path = m_configFilePath;
  const QString oldPath = backupPath();

  if (!QFile::exists(oldPath)) {
    QMessageBox::warning(this, "No Backup", "No backup file found:\n" + oldPath);
    return;
  }

  QFile::remove(path);
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
  QSettings("HyprMonitor", "HyprMonitor").setValue("configFile", path);
  m_loadedRules = HyprClient::parseRulesFile(path, &m_configHeader);
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
    m_rulesTableWidget->setItem(i, 6, new QTableWidgetItem(rule.opacity));

    QPushButton *removeButton = new QPushButton("Remove", this);

    removeButton->setFocusPolicy(Qt::NoFocus);
    removeButton->setStyleSheet(
        "QPushButton { background-color: #8f3a42; color: #ffffff;"
        " border: 1px solid #a4454e; border-radius: 6px; padding: 2px; }"
        "QPushButton:hover { background-color: #a4454e;"
        " border: 1px solid #bb545e; }");
    removeButton->setFixedHeight(28);
    removeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(removeButton, &QPushButton::clicked, this, [this, i]() {
      m_loadedRules.removeAt(i);
      closeRuleForm();
      refreshRulesTable();
    });

    QWidget *cellWrapper = new QWidget(this);
    cellWrapper->setStyleSheet("background: transparent;");
    QHBoxLayout *cellLayout = new QHBoxLayout(cellWrapper);
    cellLayout->setContentsMargins(0, 0, 0, 0);

    cellLayout->addStretch(1);
    cellLayout->addWidget(removeButton, 8);
    cellLayout->addStretch(1);
    m_rulesTableWidget->setCellWidget(i, 7, cellWrapper);
  }
}
