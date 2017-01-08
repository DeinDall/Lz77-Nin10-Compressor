#ifndef STK_LZ77_H
#define STK_LZ77_H

#include <QByteArray>

namespace Stk {

/*!
 * \brief Stk::Lz77
 *
 * Allows Nintendo 0x10-LZ77 (de)compression to happen.
 * Heavily based on Nintenlord's sources. So thanks a lot to him :)
 *
 */
class Lz77 {
protected:
	struct SearchResult {
		int start, size;
	};

public:
	Lz77();

	QByteArray compress(const QByteArray input) const;
	QByteArray decompress(const QByteArray input) const;

protected:
	SearchResult search(const QByteArray input, int position) const;

private:
	const int mSlidingWindowSize;
	const int mReadAheadBufferSize;
	const int mBlockSize;
};

} // namespace Stk

#endif // STK_LZ77_H
