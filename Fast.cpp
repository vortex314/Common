#include "Fast.h"

Fast::Fast(uint32_t size) {
    _start = new uint32_t[size];
    _offset = 0;
    _capacity = size;
    _used = 0;
}

Fast::~Fast() {}

int Fast::append(uint32_t w) {
    if (_offset < _capacity) {
        _used++;
        _offset++;
        *(_start + _offset) = w;
        return 0;
    }
    return ENOSPC;
}

int Fast::append(uint8_t* bytes, uint32_t length) { return 0; }

bool Fast::isType(Type type) {
    uint32_t length;
    Type t;
    int erc = peek(t, length);
    if (erc)
        return false;
    return (t == type);
}

int Fast::next(uint32_t& w) {
    if (_offset < _used) {
        w = *(_start + _offset);
        return 0;
    }
    return ENODATA;
}

int Fast::peek(Type& type, uint32_t& length) {
    uint32_t w = 0;
    int erc = next(w);
    if (erc)
        return erc;
    type = (Type)(w >> 24);
    length = w & 0xFFFF;
    return 0;
}

int Fast::write(std::string& s) {
    int erc;
    uint32_t size = s.size();
    erc = append((STRING << 24) + size);
    if (erc)
        return erc;
    erc = append((uint8_t*)s.data(), size);
    return erc;
}

int Fast::read(std::string& s) {
    if (isType(STRING)) {
        uint32_t w;
        int erc = next(w);
        if (erc)
            return erc;
        s.reserve(SIZEOF(w));

        return 0;
    }
    return ENOENT;
}
