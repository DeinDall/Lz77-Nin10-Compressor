#include "qstdio.h"

#include <cstdio>

QStdIO::QStdIO(QObject* parent)
	: QIODevice(parent) {
	open(QIODevice::ReadWrite);
}

QStdIO::~QStdIO() {
	if (isOpen())
		close();
}

qint64 QStdIO::writeData(const char* data, qint64 len) {
	return std::fwrite(data, 1, len, stdout);
}

qint64 QStdIO::readData(char* data, qint64 maxlen) {
	return std::fread(data, 1, maxlen, stdin);
}
