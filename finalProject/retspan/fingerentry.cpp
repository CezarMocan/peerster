#include "fingerentry.h"

FingerEntry::FingerEntry() {
}

bool FingerEntry::operator== (const FingerEntry& other) const {
    return (succ == other.succ && \
        start == other.start && stop == other.stop);
}

FingerEntry& FingerEntry::operator= (FingerEntry other) {
    this->succ = other.succ;
    this->start = other.start;
    this->stop = other.stop;
}

