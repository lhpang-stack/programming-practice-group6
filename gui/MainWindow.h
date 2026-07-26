#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QWidget>

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

    void setupUi();
    void connectSignals();
    void setTaskControlsEnabled(bool enabled);
};

#endif
