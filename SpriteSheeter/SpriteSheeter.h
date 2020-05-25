#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SpriteSheeter.h"

class SpriteSheeter : public QMainWindow
{
    Q_OBJECT

public:
    SpriteSheeter(QWidget *parent = Q_NULLPTR);

private:
    Ui::SpriteSheeterClass ui;
};
