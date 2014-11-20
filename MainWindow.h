#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QMessageBox>
#include "DisplayWindow.h"
#include "Lights/POINTLight.h"

#include "rendererpanel.h"


class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void renderScene();

private:
    DisplayWindow display;
    QVBoxLayout* mainLayout;
    QHBoxLayout* layout;
    QPushButton* renderButton;
    Image* m_image;
    QTimer timer;

    RendererPanel* m_rendererPanel;
};

#endif // MAINWINDOW_H
