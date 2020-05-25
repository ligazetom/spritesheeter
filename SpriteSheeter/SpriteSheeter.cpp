#include "SpriteSheeter.h"

bool SortFunction(int i, int j, QRegExp numberRegExp, QStringList filesFrom)
{
	numberRegExp.indexIn(filesFrom[i]);	
	int orderI = numberRegExp.cap(1).toInt();
	numberRegExp.indexIn(filesFrom[j]);
	int orderJ = numberRegExp.cap(1).toInt();

	return orderI < orderJ;	
}

SpriteSheeter::SpriteSheeter(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	m_fromDialog.setFileMode(QFileDialog::ExistingFiles);
	m_fromDialog.setNameFilter("PNG(*.png)");

	m_toDialog.setFileMode(QFileDialog::DirectoryOnly);

	m_numberRegExp = QRegExp("_([0-9]+)\\.png$");
	m_nameRegExp = QRegExp("([^/]+_[0-9]+.png)");
}

void SpriteSheeter::on_pushButtonBrowseFrom_pressed()
{	
	m_fromDialog.exec();
	m_filesFrom = m_fromDialog.selectedFiles();
	
	QString fileNames("");

	for (int i = 0; i < m_filesFrom.size() - 1; i++)
	{
		m_nameRegExp.indexIn(m_filesFrom[i]);

		if (m_nameRegExp.cap(1) == "")
		{
			ui.lineEditFrom->setText("");
			return;
		}

		fileNames += m_nameRegExp.cap(1) + "; ";		
	}

	if (m_filesFrom.size() > 0)
	{
		m_nameRegExp.indexIn(m_filesFrom.back());

		fileNames += m_nameRegExp.capturedTexts()[0];
	}

	ui.lineEditFrom->setText(fileNames);
}

void SpriteSheeter::on_pushButtonBrowseTo_pressed()
{
	m_toDialog.exec();
	m_directoryTo = m_toDialog.directory().absolutePath();

	ui.lineEditTo->setText(m_directoryTo);
}

void SpriteSheeter::on_pushButtonConvert_pressed()
{
	FillMetaData();
	m_imageSequence.clear();
	m_imageOrder.clear();
	m_imageSequence.reserve(m_filesFrom.size());
	m_imageOrder.resize(m_filesFrom.size());

	int requiredWidth = m_finalWidth / m_columnCount;
	QSize imageSize(requiredWidth, requiredWidth);

	int numOfRows = m_finalHeight / requiredWidth;

	if (m_filesFrom.size() > numOfRows * m_columnCount)
	{
		m_errorBox.setWindowTitle("Too many files");
		m_errorBox.setText("You have selected more images than you have room for in the final image.\n"
			"Change sprite sheet dimensions or select less images.");
		m_errorBox.exec();
	}

	for (int i = 0; i < m_filesFrom.size(); i++)
	{
		m_imageSequence.push_back(QImage(m_filesFrom[i]));
		m_imageSequence[i] = m_imageSequence[i].scaled(imageSize);

		m_numberRegExp.indexIn(m_filesFrom[i]);

		if (m_numberRegExp.cap(1) != "")
		{
			int order = m_numberRegExp.cap(1).toInt();
			m_imageOrder[i] = i;
		}
		else
		{
			m_errorBox.setWindowTitle("Wrong naming convention");
			m_errorBox.setText("One of the selected files has wrong numbering format.\nBe sure they end with '_*number*.png'");
			m_errorBox.exec();
			return;
		}
	}

	m_spriteSheet = QImage(m_finalWidth, m_finalHeight, QImage::Format_ARGB32_Premultiplied);

	std::sort(m_imageOrder.begin(), m_imageOrder.end(), [this](int i, int j) { return SortFunction(i, j, this->m_numberRegExp, this->m_filesFrom); });

	for (int i = 0; i < m_imageSequence.size(); i++)
	{
		InsertToImage(m_imageSequence[m_imageOrder[i]], m_spriteSheet, requiredWidth * (i % m_columnCount), requiredWidth * (i / m_columnCount));
	}

	QString fileName = m_directoryTo + "\\" + ui.lineEditImageName->text() + ".png";

	m_spriteSheet.save(fileName);
}

void SpriteSheeter::FillMetaData()
{
	m_finalWidth = ui.spinBoxWidth->value();
	m_finalHeight = ui.spinBoxHeight->value();
	m_columnCount = ui.spinBoxImagesPerRow->value();
}

void SpriteSheeter::ResetContainers()
{
	m_imageSequence.clear();
	m_filesFrom.clear();
	m_directoryTo = "";
	m_imageOrder.clear();
}

void SpriteSheeter::InsertToImage(const QImage& from, QImage& to, int x, int y) const
{
	const uchar *fromBytes = from.constBits();
	uchar *toBytes = to.bits();
	int width = from.width();
	int height = from.height();
	int fromBytesPerLine = from.bytesPerLine();
	int toBytesPerLine = to.bytesPerLine();
	
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < fromBytesPerLine; i += 4)
		{
			toBytes[(j + y) * toBytesPerLine + (i + x * 4)] = fromBytes[j * fromBytesPerLine + i];
			toBytes[(j + y) * toBytesPerLine + (i + x * 4) + 1] = fromBytes[j * fromBytesPerLine + i + 1];
			toBytes[(j + y) * toBytesPerLine + (i + x * 4) + 2] = fromBytes[j * fromBytesPerLine + i + 2];
			toBytes[(j + y) * toBytesPerLine + (i + x * 4) + 3] = fromBytes[j * fromBytesPerLine + i + 3];
		}
	}
}