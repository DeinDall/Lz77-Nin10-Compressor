#ifndef QSTDOUT_H
#define QSTDOUT_H

#include <QIODevice>

class QStdIO : public QIODevice {
	Q_OBJECT
public:
	QStdIO(QObject* parent = nullptr);
	~QStdIO();

protected:
	qint64 readData(char* data, qint64 maxlen);
	qint64 writeData(const char* data, qint64 len);
};

#endif // QSTDOUT_H
