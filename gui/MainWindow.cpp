#include "MainWindow.h"

#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <Qt>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      centralWidget(nullptr),
      usernameEdit(nullptr),
      passwordEdit(nullptr),
      loginButton(nullptr),
      loginStatusLabel(nullptr),
      keywordEdit(nullptr),
      statusFilterBox(nullptr),
      priorityFilterBox(nullptr),
      searchButton(nullptr),
      clearFilterButton(nullptr),
      taskTable(nullptr),
      taskNameEdit(nullptr),
      startTimeEdit(nullptr),
      priorityEdit(nullptr),
      categoryEdit(nullptr),
      remindTimeEdit(nullptr),
      addTaskButton(nullptr),
      editTaskButton(nullptr),
      deleteTaskButton(nullptr),
      doneTaskButton(nullptr),
      refreshButton(nullptr),
      operationStatusLabel(nullptr),
      reminderTimer(nullptr) {
    setupUi();
    connectSignals();
    setTaskControlsEnabled(false);
}

void MainWindow::setupUi() {
    setWindowTitle("MySchedule Qt GUI - Linux Edition");
    resize(1150, 720);
    setMinimumSize(1000, 650);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* mainLayout = new QVBoxLayout(centralWidget);
    auto* titleLabel = new QLabel("MySchedule Task Manager", centralWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #203864;"
        "padding: 10px;"
    );

    mainLayout->addWidget(titleLabel);

    // Login area
    auto* loginGroup = new QGroupBox("User Login", centralWidget);
    auto* loginLayout = new QHBoxLayout(loginGroup);

    usernameEdit = new QLineEdit(loginGroup);
    usernameEdit->setPlaceholderText("Username");

    passwordEdit = new QLineEdit(loginGroup);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Login", loginGroup);
    loginStatusLabel = new QLabel("Not logged in", loginGroup);

    loginLayout->addWidget(new QLabel("Username:", loginGroup));
    loginLayout->addWidget(usernameEdit);
    loginLayout->addWidget(new QLabel("Password:", loginGroup));
    loginLayout->addWidget(passwordEdit);
    loginLayout->addWidget(loginButton);
    loginLayout->addWidget(loginStatusLabel);

    mainLayout->addWidget(loginGroup);

    // Search and filter area
    auto* filterGroup = new QGroupBox("Search / Filter", centralWidget);
    auto* filterLayout = new QHBoxLayout(filterGroup);

    keywordEdit = new QLineEdit(filterGroup);
    keywordEdit->setPlaceholderText("Keyword: task name / category / priority / status");

    statusFilterBox = new QComboBox(filterGroup);
    statusFilterBox->addItem("all");
    statusFilterBox->addItem("pending");
    statusFilterBox->addItem("done");

    priorityFilterBox = new QComboBox(filterGroup);
    priorityFilterBox->addItem("all");
    priorityFilterBox->addItem("high");
    priorityFilterBox->addItem("medium");
    priorityFilterBox->addItem("low");

    searchButton = new QPushButton("Apply Filter", filterGroup);
    clearFilterButton = new QPushButton("Clear Filter", filterGroup);

    filterLayout->addWidget(new QLabel("Keyword:", filterGroup));
    filterLayout->addWidget(keywordEdit);
    filterLayout->addWidget(new QLabel("Status:", filterGroup));
    filterLayout->addWidget(statusFilterBox);
    filterLayout->addWidget(new QLabel("Priority:", filterGroup));
    filterLayout->addWidget(priorityFilterBox);
    filterLayout->addWidget(searchButton);
    filterLayout->addWidget(clearFilterButton);

    mainLayout->addWidget(filterGroup);

    // Task table
    taskTable = new QTableWidget(0, 8, centralWidget);
    taskTable->setHorizontalHeaderLabels({
        "ID",
        "Name",
        "Start Time",
        "Priority",
        "Category",
        "Remind Time",
        "Status",
        "Reminded"
    });

    taskTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    taskTable->setSelectionMode(QAbstractItemView::SingleSelection);
    taskTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mainLayout->addWidget(taskTable);

    // Add / edit task area
    auto* editGroup = new QGroupBox("Add / Edit Task", centralWidget);
    auto* editLayout = new QHBoxLayout(editGroup);

    taskNameEdit = new QLineEdit(editGroup);
    taskNameEdit->setPlaceholderText("Task name");

    startTimeEdit = new QLineEdit(editGroup);
    startTimeEdit->setPlaceholderText("YYYY-MM-DD_HH:MM");

    priorityEdit = new QLineEdit(editGroup);
    priorityEdit->setPlaceholderText("high / medium / low");

    categoryEdit = new QLineEdit(editGroup);
    categoryEdit->setPlaceholderText("study / entertainment / life");

    remindTimeEdit = new QLineEdit(editGroup);
    remindTimeEdit->setPlaceholderText("YYYY-MM-DD_HH:MM");

    addTaskButton = new QPushButton("Add", editGroup);
    editTaskButton = new QPushButton("Update Selected", editGroup);

    editLayout->addWidget(taskNameEdit);
    editLayout->addWidget(startTimeEdit);
    editLayout->addWidget(priorityEdit);
    editLayout->addWidget(categoryEdit);
    editLayout->addWidget(remindTimeEdit);
    editLayout->addWidget(addTaskButton);
    editLayout->addWidget(editTaskButton);

    mainLayout->addWidget(editGroup);

    // Operation buttons
    auto* operationLayout = new QHBoxLayout();

    deleteTaskButton = new QPushButton("Delete Selected", centralWidget);
    doneTaskButton = new QPushButton("Mark Done", centralWidget);
    refreshButton = new QPushButton("Refresh", centralWidget);
    operationStatusLabel = new QLabel("Please login first.", centralWidget);

    operationLayout->addWidget(deleteTaskButton);
    operationLayout->addWidget(doneTaskButton);
    operationLayout->addWidget(refreshButton);
    operationLayout->addWidget(operationStatusLabel);

    mainLayout->addLayout(operationLayout);

    // Timer for GUI reminder checking
    reminderTimer = new QTimer(this);
    reminderTimer->setInterval(30000);

    setStyleSheet(
        "QMainWindow { background-color: #f5f7fb; }"

        "QGroupBox { "
        "  font-weight: bold; "
        "  border: 1px solid #cfd7e6; "
        "  border-radius: 8px; "
        "  margin-top: 10px; "
        "  padding: 8px; "
        "  background-color: #ffffff; "
        "}"

        "QGroupBox::title { "
        "  subcontrol-origin: margin; "
        "  left: 10px; "
        "  padding: 0 4px; "
        "}"

        "QLineEdit, QComboBox { "
        "  padding: 6px; "
        "  border: 1px solid #b8c2d6; "
        "  border-radius: 5px; "
        "  background-color: #ffffff; "
        "}"

        "QPushButton { "
        "  padding: 7px 12px; "
        "  border-radius: 6px; "
        "  background-color: #3267d6; "
        "  color: white; "
        "  font-weight: bold; "
        "}"

        "QPushButton:hover { background-color: #2454b8; }"
        "QPushButton:disabled { background-color: #9aa6bd; }"

        "QTableWidget { "
        "  background-color: #ffffff; "
        "  gridline-color: #d8deea; "
        "  selection-background-color: #d9e7ff; "
        "  selection-color: #000000; "
        "}"

        "QHeaderView::section { "
        "  background-color: #e8eef9; "
        "  padding: 6px; "
        "  border: 1px solid #cfd7e6; "
        "  font-weight: bold; "
        "}"
    );
}

void MainWindow::connectSignals() {
}

void MainWindow::setTaskControlsEnabled(bool enabled) {
    keywordEdit->setEnabled(enabled);
    statusFilterBox->setEnabled(enabled);
    priorityFilterBox->setEnabled(enabled);
    searchButton->setEnabled(enabled);
    clearFilterButton->setEnabled(enabled);

    taskTable->setEnabled(enabled);

    taskNameEdit->setEnabled(enabled);
    startTimeEdit->setEnabled(enabled);
    priorityEdit->setEnabled(enabled);
    categoryEdit->setEnabled(enabled);
    remindTimeEdit->setEnabled(enabled);

    addTaskButton->setEnabled(enabled);
    editTaskButton->setEnabled(enabled);
    deleteTaskButton->setEnabled(enabled);
    doneTaskButton->setEnabled(enabled);
    refreshButton->setEnabled(enabled);
}
