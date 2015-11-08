#ifndef SLIP_H
#define SLIP_H
#include "stdint.h"
#include "Bytes.h"

class Slip : public Bytes
{
public:
    enum Magic {ESC=0x7D,SOF=0x7E};
    Slip(uint32_t size);
    virtual ~Slip();
    Slip& addCrc();
    Slip& removeCrc();

    Slip& encode();
    Slip& decode();
    bool isGoodCrc();
    Slip& frame();
    bool fill(uint8_t b);
protected:
private:
    static uint16_t Fletcher16(uint8_t *begin, int length);
};

#endif // SLIP_H
