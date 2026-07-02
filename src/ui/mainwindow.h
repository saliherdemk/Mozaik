#pragma once
#include "hypr_client.h"
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() = default;

private slots:
  void refreshWindowList();
  void applySelectedRule();
  void populateFormFromSelection();
  void populateFormFromRuleSelection();
  void browseConfigFile();
  void restoreOldConfig();

private:
  HyprClient m_hyprClient;
  QVector<ExistingRule> m_loadedRules;
  QString m_configFilePath;
  QString m_configHeader;
  int m_editingRuleIndex = -1;

  QLabel *m_configFileLabel;
  QPushButton *m_browseButton;
  QTableWidget *m_rulesTableWidget;

  QTableWidget *m_tableWidget;
  QPushButton *m_refreshButton;
  QPushButton *m_applyButton;

  QGroupBox *m_formGroup;

  QLineEdit *m_nameEdit;
  QLineEdit *m_matchClassEdit;
  QCheckBox *m_matchTitleCheckBox;
  QLineEdit *m_matchTitleEdit;

  QCheckBox *m_floatCheckBox;

  QCheckBox *m_sizeCheckBox;
  QSpinBox *m_sizeWidthSpin;
  QSpinBox *m_sizeHeightSpin;

  QCheckBox *m_moveCheckBox;
  QSpinBox *m_moveXSpin;
  QSpinBox *m_moveYSpin;

  void setupUi();
  void loadRulesFromFile(const QString &path);
  void refreshRulesTable();
  void closeRuleForm();
};
