#include "MainWindow.h"

#include "SoundReminder.h"
#include "Task.h"

#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include <algorithm>
#include <cctype>
#include <vector>
#include <Qt>

namespace {

std::string toLowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool containsIgnoreCase(const std::string& text, const std::string& keyword) {
    if (keyword.empty()) {
        return true;
    }

    const std::string lowerText = toLowerCopy(text);
    const std::string lowerKeyword = toLowerCopy(keyword);

    return lowerText.find(lowerKeyword) != std::string::npos;
}

} // namespace

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
      reminderTimer(nullptr),
      userManager("data/users.txt") {
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
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::handleLogin);
    connect(addTaskButton, &QPushButton::clicked, this, &MainWindow::handleAddTask);
    connect(editTaskButton, &QPushButton::clicked, this, &MainWindow::handleEditTask);
    connect(deleteTaskButton, &QPushButton::clicked, this, &MainWindow::handleDeleteTask);
    connect(doneTaskButton, &QPushButton::clicked, this, &MainWindow::handleDoneTask);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshTaskTable);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::refreshTaskTableWithFilters);
    connect(clearFilterButton, &QPushButton::clicked, this, &MainWindow::handleClearFilters);
    connect(taskTable, &QTableWidget::cellClicked, this, &MainWindow::loadSelectedTaskToEditor);
    connect(reminderTimer, &QTimer::timeout, this, &MainWindow::handleReminderCheck);
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

void MainWindow::handleLogin() {
    const QString username = usernameEdit->text().trimmed();
    const QString password = passwordEdit->text();

    if (!userManager.loginUser(username.toStdString(), password.toStdString())) {
        loginStatusLabel->setText("Login failed");
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
        return;
    }

    currentUsername = username.toStdString();
    taskManager = std::make_unique<TaskManager>(currentUsername);

    if (!taskManager->loadTasks()) {
        QMessageBox::information(this, "Load Notice", "No existing task file found. A new one will be created after adding tasks.");
    }

    loginStatusLabel->setText("Logged in: " + username);
    operationStatusLabel->setText("Login successful. GUI reminder timer started.");
    setTaskControlsEnabled(true);
    refreshTaskTable();

    reminderTimer->start();
}

void MainWindow::fillTaskTable(const std::vector<Task>& tasks) {
    taskTable->setRowCount(0);

    for (const auto& task : tasks) {
        const int row = taskTable->rowCount();
        taskTable->insertRow(row);

        taskTable->setItem(row, 0, new QTableWidgetItem(QString::number(task.id)));
        taskTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(task.name)));
        taskTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(task.startTime)));
        taskTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(task.priority)));
        taskTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(task.category)));
        taskTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(task.remindTime)));
        taskTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(task.status)));
        taskTable->setItem(row, 7, new QTableWidgetItem(task.reminded ? "yes" : "no"));
    }
}

void MainWindow::refreshTaskTable() {
    if (!taskManager) {
        return;
    }

    taskManager->loadTasks();

    std::vector<Task> tasks = taskManager->getTasks();

    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.startTime < b.startTime;
    });

    fillTaskTable(tasks);
    operationStatusLabel->setText("Task list refreshed.");
}

void MainWindow::refreshTaskTableWithFilters() {
    if (!taskManager) {
        return;
    }

    taskManager->loadTasks();

    const std::string keyword = keywordEdit->text().trimmed().toStdString();
    const std::string statusFilter = statusFilterBox->currentText().toStdString();
    const std::string priorityFilter = priorityFilterBox->currentText().toStdString();

    std::vector<Task> tasks = taskManager->getTasks();
    std::vector<Task> filteredTasks;

    for (const auto& task : tasks) {
        bool matched = true;

        if (statusFilter != "all" && task.status != statusFilter) {
            matched = false;
        }

        if (priorityFilter != "all" && task.priority != priorityFilter) {
            matched = false;
        }

        const bool keywordMatched =
            containsIgnoreCase(task.name, keyword) ||
            containsIgnoreCase(task.category, keyword) ||
            containsIgnoreCase(task.priority, keyword) ||
            containsIgnoreCase(task.status, keyword) ||
            containsIgnoreCase(task.startTime, keyword) ||
            containsIgnoreCase(task.remindTime, keyword);

        if (!keywordMatched) {
            matched = false;
        }

        if (matched) {
            filteredTasks.push_back(task);
        }
    }

    std::sort(filteredTasks.begin(), filteredTasks.end(), [](const Task& a, const Task& b) {
        return a.startTime < b.startTime;
    });

    fillTaskTable(filteredTasks);

    operationStatusLabel->setText(
        "Filter applied. Matched tasks: " + QString::number(static_cast<int>(filteredTasks.size()))
    );
}

void MainWindow::handleClearFilters() {
    keywordEdit->clear();
    statusFilterBox->setCurrentText("all");
    priorityFilterBox->setCurrentText("all");

    refreshTaskTable();
    operationStatusLabel->setText("Filters cleared.");
}

void MainWindow::handleAddTask() {
    if (!taskManager) {
        return;
    }

    const std::string name = taskNameEdit->text().trimmed().toStdString();
    const std::string startTime = startTimeEdit->text().trimmed().toStdString();
    std::string priority = priorityEdit->text().trimmed().toStdString();
    std::string category = categoryEdit->text().trimmed().toStdString();
    const std::string remindTime = remindTimeEdit->text().trimmed().toStdString();

    // DELIBERATE BUG: Missing check for empty task name and start time here.

    if (priority.empty()) {
        priority = "medium";
    }

    if (category.empty()) {
        category = "life";
    }

    const bool ok = taskManager->addTask(
        name,
        startTime,
        priority,
        category,
        remindTime
    );

    if (!ok) {
        QMessageBox::warning(
            this,
            "Add Failed",
            "Failed to add task. Please check time format or uniqueness constraints."
        );
        return;
    }

    taskNameEdit->clear();
    startTimeEdit->clear();
    priorityEdit->clear();
    categoryEdit->clear();
    remindTimeEdit->clear();

    operationStatusLabel->setText("Task added successfully.");
    refreshTaskTable();
}

int MainWindow::getSelectedTaskId() const {
    const int row = taskTable->currentRow();

    if (row < 0) {
        return -1;
    }

    QTableWidgetItem* idItem = taskTable->item(row, 0);

    if (!idItem) {
        return -1;
    }

    bool ok = false;
    const int id = idItem->text().toInt(&ok);

    return ok ? id : -1;
}

void MainWindow::loadSelectedTaskToEditor() {
    const int row = taskTable->currentRow();

    if (row < 0) {
        return;
    }

    auto getText = [this, row](int column) -> QString {
        QTableWidgetItem* item = taskTable->item(row, column);
        return item ? item->text() : QString();
    };

    taskNameEdit->setText(getText(1));
    startTimeEdit->setText(getText(2));
    priorityEdit->setText(getText(3));
    categoryEdit->setText(getText(4));
    remindTimeEdit->setText(getText(5));

    operationStatusLabel->setText("Selected task loaded into editor.");
}

void MainWindow::handleEditTask() {
    if (!taskManager) {
        return;
    }

    const int id = getSelectedTaskId();

    if (id < 0) {
        QMessageBox::warning(this, "Update Failed", "Please select a task first.");
        return;
    }

    const std::string name = taskNameEdit->text().trimmed().toStdString();
    const std::string startTime = startTimeEdit->text().trimmed().toStdString();
    const std::string priority = priorityEdit->text().trimmed().toStdString();
    const std::string category = categoryEdit->text().trimmed().toStdString();
    const std::string remindTime = remindTimeEdit->text().trimmed().toStdString();

    // DELIBERATE BUG: Missing check for empty task name and start time here.

    bool ok = true;
    ok = ok && taskManager->editTask(id, "name", name);
    ok = ok && taskManager->editTask(id, "startTime", startTime);
    ok = ok && taskManager->editTask(id, "priority", priority.empty() ? "medium" : priority);
    ok = ok && taskManager->editTask(id, "category", category.empty() ? "life" : category);
    ok = ok && taskManager->editTask(id, "remindTime", remindTime);

    if (!ok) {
        QMessageBox::warning(
            this,
            "Update Failed",
            "Failed to update task. Please check time format or uniqueness constraints."
        );
        refreshTaskTable();
        return;
    }

    operationStatusLabel->setText("Task updated successfully.");
    refreshTaskTable();
}

void MainWindow::handleDeleteTask() {
    if (!taskManager) {
        return;
    }

    const int id = getSelectedTaskId();

    if (id < 0) {
        QMessageBox::warning(this, "Delete Failed", "Please select a task first.");
        return;
    }

    const auto result = QMessageBox::question(
        this,
        "Confirm Delete",
        "Delete selected task?"
    );

    if (result != QMessageBox::Yes) {
        return;
    }

    if (!taskManager->deleteTaskById(id)) {
        QMessageBox::warning(this, "Delete Failed", "Failed to delete task.");
        return;
    }

    operationStatusLabel->setText("Task deleted successfully.");
    refreshTaskTable();
}

void MainWindow::handleDoneTask() {
    if (!taskManager) {
        return;
    }

    const int id = getSelectedTaskId();

    if (id < 0) {
        QMessageBox::warning(this, "Mark Done Failed", "Please select a task first.");
        return;
    }

    if (!taskManager->markTaskDone(id)) {
        QMessageBox::warning(this, "Mark Done Failed", "Failed to mark task as done.");
        return;
    }

    operationStatusLabel->setText("Task marked as done.");
    refreshTaskTable();
}

void MainWindow::handleReminderCheck() {
    if (!taskManager) {
        return;
    }

    taskManager->loadTasks();
    taskManager->checkAndRemindTasks();
    refreshTaskTableWithFilters();
}
