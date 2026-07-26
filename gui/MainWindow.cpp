#include "MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <Qt>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      centralWidget(nullptr) {
    setupUi();
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
}
