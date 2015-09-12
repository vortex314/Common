#include <iostream>
#include "stdio.h"

using namespace std;

#include "Cbor.h"
#include "Sys.h"

bool cborUInt64(uint64_t ll,const char* result)
{
    Cbor cbor(100);
    Str str(200);

    cbor.add(ll);
    ((Bytes)cbor).toString(str);
    INFO("%Lu : result : >%s< : expected : >%s<  ",ll,str.c_str(),result);
    return true;
}
bool cborInt64(int64_t ll,const char* result)
{
    Cbor cbor(100);
    Str str(200);

    cbor.add(ll);
    ((Bytes)cbor).toString(str);
    INFO("%Ld : result : >%s< : expected : >%s<  ",ll,str.c_str(),result);
    return true;
}

bool cborFloat(float f,const char* result)
{
    Cbor cbor(100);
    Str str(200);

    cbor.add(f);
    ((Bytes)cbor).toString(str);
    INFO("%f : result : >%s< : expected : >%s<  ",f,str.c_str(),result);
    return true;
}

bool testCbor()
{
    Cbor cbor(100);
    Str str(200);

    cborUInt64(0,"0x00");
    cborUInt64( 1,"0x01");
    cborUInt64( 10,"0x0a");
    cborUInt64(23 ,"0x17");
    cborUInt64( 24 ,"0x1818");
    cborUInt64(25 ,"0x1819"   );
    cborUInt64( 100 ,"0x1864" );
    cborUInt64( 1000 ,"0x1903e8"   );
    cborUInt64(1000000,"0x1a000f4240" );
    cborUInt64( 1000000000000 ,"0x1b000000e8d4a51000" );
    cborUInt64( 18446744073709551615ULL ,"0x1bffffffffffffffff"   );
//    cborUInt64( 18446744073709551616ULL,"0xc249010000000000000000"  );
//   cborInt64(-18446744073709551616LL,"0x3bffffffffffffffff" );
//   cborInt64( -18446744073709551617LL,"0xc349010000000000000000"  );
    cborInt64( -1 ,"0x20");
    cborInt64( -10 ,"0x29");
    cborInt64( -100,"0x3863");
    cborInt64( -1000 ,"0x3903e7");
    cborFloat(3.4028234663852886e+38,"0xfa7f7fffff");

    /*
       |                              |                                    |
       |                           |                            |
       |                              |                                    |
       |                       |                            |
       |                              |                                    |
       | 100000.0                     | 0xfa47c35000                       |
       |                              |                                    |
       | 3.4028234663852886e+38       | 0xfa7f7fffff                       |
       |                              |                                    |
       | 1.0e+300                     | 0xfb7e37e43c8800759c               |
       |                              |                                    |
       | 5.960464477539063e-8         | 0xf90001                           |
       |                              |                                    |
       | 0.00006103515625             | 0xf90400                           |
       |                              |                                    |
       | -4.0                         | 0xf9c400                           |
       |                              |                                    |
       | -4.1                         | 0xfbc010666666666666
    */
    cbor.clear();
    cbor.add(true);
    cbor.add(1234);
    cbor.add("Hi");
    cbor.toString(str);
    INFO(str.c_str());
    str.clear();
    cbor.clear();
    str="This is a String class";
    INFO("cbor.addf(\"isuSB\",1234,\"Hidihi\",123,&str,(Bytes*)&str)");
    cbor.addf("isuSB",1234,"Hidihi",123,&str,(Bytes*)&str);
    str.clear();
    cbor.toString(str);
    INFO(str.c_str());

    cbor.offset(0);
    int32_t i;
    uint32_t ui;
    char s[100];
    Str str2(100);
    Bytes bytes2(100);
    INFO("Reading from cbor.scanf ");
    if ( cbor.scanf("isuSB",&i,s,100,&ui,&str2,&bytes2) ) INFO("scanf succeeded.");
    else INFO("scanf failed.");
    INFO("%d %s %u %s ",i,s,ui,str2.c_str());
    str2.clear();
    bytes2.toString(str2);
    INFO("Bytes : %s",str2.c_str());

    str="The quick brown fox";
    int max=100000;

    INFO("Benchmark Test : %d",max);
    for(int count=0; count<max; count++)
    {
        cbor.clear();
        if ( cbor.addf("isuSB",1234,"Hidihi",123,&str,(Bytes*)&str)==false)
        {
            INFO("addf fails");
            break;
        };
        cbor.offset(0);
        if ( cbor.scanf("isuSB",&i,s,100,&ui,&str2,&bytes2)==false)
        {
            INFO("scanf fails");
            break;
        };
        str2.clear();
        bytes2.clear();
    }
    return true;
}


#include "Slip.h"
bool testSlip()
{
    Slip bytes(100);

    for (int i = 0; i < 254; i++)
    {
        bytes.clear();
        bytes.write((uint8_t) i);
        bytes.write((uint8_t) i + 1);
        bytes.write((uint8_t) i + 2);
        bytes.encode();
        bytes.addCrc();
        if (bytes.isGoodCrc())
        {
            bytes.removeCrc();
            bytes.decode();
            bytes.offset(0);
            if (i != bytes.read())
                return false;
            if ((i + 1) != bytes.read())
                return false;
            if ((i + 2) != bytes.read())
                return false;
        }
        else
            return false;
    }
    return true;
}

#include "Slip.h"

bool slipTest()
{
    Slip slip(512);
    for(int i=0; i< 256; i++) slip.write(i);
    slip.addCrc();
    slip.encode();
    slip.decode();
    if ( slip.isGoodCrc() == false )
    {
        INFO(" Invalid CRC on buffer ");
        return false;
    }
    for(int i=0; i< 256; i++)
    {
        uint8_t b = slip.read();
        if ( b != i )
        {
            INFO(" Slip data changed ");
            return false;
        }
    }
    return true;
}

#include "Msg.h"

bool testMsg()
{
    Msg::init();

    Msg msg(0);
    int max=1000000;
    INFO(" testing Msg times : %d",max);
    for(int i=0; i<max; i++)
    {
        msg.create(&msg,SIG_RXD).add(i).add("Hi").add(0xDEADBEEF);
        msg.send();

        msg.create(&msg,SIG_TXD) << i+1 << "Hi" << 0xDEADBEEF; // .add(i+1).add("Hi").add(0xDEADBEEF)
        msg.send();

        msg.create(&msg,SIG_RXD).add(i).add("Hi").add(0xDEADBEEF);
        msg.send();

        msg.create(&msg,SIG_TXD) << i+1 << "Hi" << 0xDEADBEEF; // .add(i+1).add("Hi").add(0xDEADBEEF)
        msg.send();

        msg.create(&msg,SIG_TXD).addf("isu",i, "Hi", 0xDEADBEEF); // .add(i+1).add("Hi").add(0xDEADBEEF)
        msg.send();



//       msg.alloc(&msg,SIG_CONNECTED).addf("isu",i,"Hi",0xDEADBEEF);
//       msg.send();

        uint32_t j;
        char str[10];

        while ( msg.receive())
        {
            msg.get(j);
            if ( j != i &&  j != i+1  ) return false;
            msg.get(str,10);
            if ( strcmp("Hi",str)!=0) return false;
            msg.get(j);
            if ( j != 0xDEADBEEF) return false;
            msg.free();
        }
    }
    return true;
}


int main()
{
    INFO("Testing Msg...");
    if ( testMsg())    INFO("Test OK.");
    else INFO("Test Failed !!.");
    INFO("Testing Cbor...");
    if ( testCbor())    INFO("Test OK.");
    else INFO("Test Failed !!.");
    INFO("Testing Slip...");
    if ( testSlip())    INFO("Test OK.");
    else INFO("Test Failed !!.");
    return 0;
}



