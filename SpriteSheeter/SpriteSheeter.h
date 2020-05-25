#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SpriteSheeter.h"
#include <QImage>
#include <QList>
#include <QStringList>
#include <QFileDialog>
#include <QRegExp>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <vector>
#include <QSize>
#include <algorithm>

class SpriteSheeter : public QMainWindow
{
    Q_OBJECT

public:
    SpriteSheeter(QWidget *parent = Q_NULLPTR);

private:
    Ui::SpriteSheeterClass ui;
	QFileDialog m_fromDialog;
	QFileDialog m_toDialog;
	QImage m_spriteSheet;
	QList<QImage> m_imageSequence;
	QString m_directoryTo;
	QStringList m_filesFrom;
	std::vector<int> m_imageOrder;
	QRegExp m_numberRegExp;
	QRegExp m_nameRegExp;
	QMessageBox m_errorBox;
	int m_finalWidth;
	int m_finalHeight;
	int m_columnCount;

	void ResetContainers();
	void FillMetaData();
	void InsertToImage(const QImage& from, QImage& to, int x, int y) const;

private slots:
	void on_pushButtonBrowseFrom_pressed();
	void on_pushButtonBrowseTo_pressed();
	void on_pushButtonConvert_pressed();
};
