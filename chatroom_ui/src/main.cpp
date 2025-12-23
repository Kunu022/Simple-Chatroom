#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QListWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QStackedWidget>
#include <QStackedLayout>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QButtonGroup>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>

#include "ChatWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ChatWindow window;
    window.show();

    return app.exec();
}