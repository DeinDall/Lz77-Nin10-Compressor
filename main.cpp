#include <QCoreApplication>

#include "stk/lz77.h"
#include "qstdio.h"

#include <QFileInfo>
#include <QStringBuilder>

enum RunResult {
	RunSuccess = 0,
	RunFailure = 1
};

struct Configuration {
	bool toStdOut;
	QString inFile;
	QString outFile;
};

QString getDefaultOutFile(QString infile) {
	QFileInfo info(infile);
	return info.path() % "/" % info.baseName() % ".lz77.dmp";
}

int run(Configuration config) {
	Stk::Lz77 lz77;

	QByteArray data; {
		QFile file(config.inFile);

		if (!file.exists())
			return RunFailure;

		if (!file.open(QIODevice::ReadOnly))
			return RunFailure;

		data = file.readAll();
		file.close();
	}

	data = lz77.compress(data);

	if (config.toStdOut) {
		QStdIO qStdIO;

		//qStdIO.write(lz77.decompress(data));
		qStdIO.write(data);
	} else {
		if (config.outFile.isEmpty())
			config.outFile = getDefaultOutFile(config.inFile);

		QFile file(config.outFile);

		if (!file.open(QIODevice::WriteOnly))
			return RunFailure;

		file.write(data);
		file.close();
	}

	return RunSuccess;
}

int main(int argc, char** argv) {
	QCoreApplication app(argc, argv);

	Configuration config = { false, QString(), QString() }; {
		QStringList arguments = QCoreApplication::arguments();

		if (arguments.first().endsWith(QFileInfo(QCoreApplication::applicationFilePath()).fileName()))
			arguments.removeFirst();

		for (int i=0; i<arguments.size(); ++i) {
			if (arguments.at(i) == "--to-stdout")
				config.toStdOut = true;
			else if (arguments.at(i) == "-o") {
				if ((++i) < arguments.size())
					config.outFile = arguments.at(i);
			} else
				config.inFile = arguments.at(i);
		}
	}

	return run(config);
}
