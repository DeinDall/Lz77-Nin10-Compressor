#include "lz77.h"

#include <QVector>

namespace Stk {

Lz77::Lz77()
	: mSlidingWindowSize(4096), mReadAheadBufferSize(18), mBlockSize(8) {}

QByteArray Lz77::compress(const QByteArray input) const {
	int position = 0;

	QByteArray output;

	{
		union {
			quint32 data;
			char bytes[4];
		} header;

		header.data = (input.size() << 8) | 0x10;
		output.append(header.bytes, 4);
	}

	while (position < input.size()) {
		quint8 compressionFlags = 0;

		QByteArray newBytes;
		newBytes.reserve(mBlockSize * 2);

		for (int i=0; i<mBlockSize; ++i) {
			if (position >= input.size())
				break;

			SearchResult searchResult = search(input, position);

			if (searchResult.size > 2) {
				newBytes.append(((((searchResult.size-3) & 0xF) << 4) | (((searchResult.start-1) >> 8) & 0xF)));
				newBytes.append(((searchResult.start-1) & 0xFF));

				position += searchResult.size;
				compressionFlags |= (quint8) (1 << (mBlockSize-(i+1)));
			} else
				newBytes.append(input.at(position++));
		}

		output.append(compressionFlags);
		output.append(newBytes);
	}

	if (int div = output.size()%4)
		output.append(4-div, (char) 0x00);

	return output;
}

QByteArray Lz77::decompress(const QByteArray input) const {
	auto it = input.begin();

	if (*(it++) != 0x10)
		return QByteArray();

	QByteArray output;

	int position = 0;
	int size = 0; {
		quint8 bytes[3];

		bytes[0] = *(it++);
		bytes[1] = *(it++);
		bytes[2] = *(it++);

		size = (bytes[0]) | (bytes[1]<<8) | (bytes[2]<<16);
	}

	output.resize(size);

	while (position < size) {
		quint8 compressionFlags = *(it++);

		for (int i=0; i<mBlockSize; ++i) {
			if (compressionFlags & 0x80) {
				quint8 lbyte = *(it++);
				quint8 rbyte = *(it++);

				int copySize = 3+(lbyte>>4);
				int copyPos = 1+(((lbyte & 0x0F) << 8) | rbyte);

				if (copyPos > size) // Error
					return QByteArray();

				for (int j=0; j<copySize; ++j) {
					output[position] = output[position-j - copyPos+(j%copyPos)];
					++position;
				}
			} else
				output[position++] = *(it++);

			if (position >= size)
				break;

			compressionFlags = compressionFlags << 1;
		}
	}

	return output;
}

Lz77::SearchResult Lz77::search(const QByteArray input, int position) const {
	if (position + 2 >= input.size())
		return { 0, 0 };
	if (position < 2)
		return { 0, 0 };

	QVector<int> results;

	// Find any potential result
	for (int i=1; i < qMin<int>(mSlidingWindowSize, position); ++i)
		if (input.at(position - (i+1)) == input.at(position))
			results.append(i+1);

	if (results.isEmpty())
		return { 0, 0 };

	int byteCount = 0;

	// Eliminate results until having either only one or only results with max matches
	while ((results.size() > 1) && (byteCount < mReadAheadBufferSize)) {
		++byteCount;

		for (int i = results.size()-1; i>=0; --i) {
			if (input[position+byteCount] != input[position - results[i] + (byteCount % results[i])]) {
				if (results.size() > 1)
					results.removeAt(i);
				else
					break;
			}
		}

		if (position+byteCount+1 == input.size())
			break;
	}

	return { results[0], byteCount };
}

} // namespace Stk
