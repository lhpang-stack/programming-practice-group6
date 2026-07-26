#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "TaskManager.h"
#include "UserManager.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QWidget>

#include <memory>
#include <string>
#include <vector>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    QWidget* centralWidget;

    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginButton;
    QLabel* loginStatusLabel;

    QLineEdit* keywordEdit;
    QComboBox* statusFilterBox;
    QComboBox* priorityFilterBox;
    QPushButton* searchButton;
    QPushButton* clearFilterButton;

    QTableWidget* taskTable;

    QLineEdit* taskNameEdit;
    QLineEdit* startTimeEdit;
    QLineEdit* priorityEdit;
    QLineEdit* categoryEdit;
    QLineEdit* remindTimeEdit;

    QPushButton* addTaskButton;
    QPushButton* editTaskButton;
    QPushButton* deleteTaskButton;
    QPushButton* doneTaskButton;
    QPushButton* refreshButton;

    QLabel* operationStatusLabel;

    QTimer* reminderTimer;

    UserManager userManager;
    std::unique_ptr<TaskManager> taskManager;
    std::string currentUsername;

    void setupUi();
    void connectSignals();

    void handleLogin();
    void handleAddTask();
    void handleEditTask();
    void handleDeleteTask();
    void handleDoneTask();

    void refreshTaskTable();
    void refreshTaskTableWithFilters();
    void fillTaskTable(const std::vector<Task>& tasks);

    void handleClearFilters();
    void handleReminderCheck();

    int getSelectedTaskId() const;
    void loadSelectedTaskToEditor();
    void setTaskControlsEnabled(bool enabled);
};

#endif
