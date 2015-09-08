#include <iostream>
#include "stdio.h"

using namespace std;

#include "Cbor.h"
#include "Sys.h"

bool cborInt(int64_t ll,const char* result)
{
    Cbor cbor(100);
    Str str(200);

    cbor.add(ll);
    ((Bytes)cbor).toString(str);
    INFO("%Ld : >%s< : >%s<  ",ll,str.c_str(),result);
    return true;
}

bool testCbor()
{
    Cbor cbor(100);
    Str str(200);

    cborInt(0,"0x00");
    cborInt( 1,"0x01");
    cborInt( 10,"0x0a");
    cborInt(23 ,"0x17");
    cborInt( 24 ,"0x1818");
    cborInt(25 ,"0x1819"   );
    cborInt( 100 ,"0x1864" );
    cborInt( 1000 ,"0x1903e8"   );
    cborInt(1000000,"0x1a000f4240" );
    cborInt( 1000000000000 ,"0x1b000000e8d4a51000" );
    cborInt( 18446744073709551615L ,"0x1bffffffffffffffff"   );
    cborInt( 18446744073709551616L,"0xc249010000000000000000"  );
    cborInt(-18446744073709551616L,"0x3bffffffffffffffff" );
    cborInt( -18446744073709551617L,"0xc349010000000000000000"  );
    cborInt( -1 ,"0x20");
    cborInt( -10 ,"0x29");
    cborInt( -100,"0x3863");
    cborInt( -1000 ,"0x3903e7");

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
    if ( cbor.scanf("isuSB",&i,s,&ui,&str2,&bytes2) ) INFO("scanf succeeded.");
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
        if ( cbor.scanf("isuSB",&i,s,&ui,&str2,&bytes2)==false)
        {
            INFO("scanf fails");
            break;
        };
        str2.clear();
        bytes2.clear();
    }
    return true;
}

int main()
{
    printf("Testing Cbor...\n");
    if ( testCbor())    INFO("Test OK.");
    else INFO("Test Failed !!.");
    return 0;
}
