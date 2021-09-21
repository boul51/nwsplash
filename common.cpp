#include "common.h"

namespace common {

const QByteArray& frameEndMarker()
{
    static QByteArray ba;
    if (ba.size() == 0) {
        ba.append(static_cast<uint8_t>(0x00));
        ba.append(static_cast<uint8_t>(0x00));
    }

    return ba;
}

}  // namespace common
