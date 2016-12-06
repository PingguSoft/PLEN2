/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG true

#include <Arduino.h>
#include <SPI.h>
#include "Sd2Card.h"
#include "Pin.h"
#include "ExternalSD.h"

#if DEBUG
    #include "System.h"
    #include "Profiler.h"
#endif


static Sd2Card   mSDCard;
static bool      mCacheDirty = false;
static uint32_t  mCacheBlockNumber = 0XFFFFFFFF;
static uint8_t   mCacheBuf[512];

// value for action argument in cacheRawBlock to indicate read from cache
static uint8_t const CACHE_FOR_READ = 0;
// value for action argument in cacheRawBlock to indicate cache dirty
static uint8_t const CACHE_FOR_WRITE = 1;

bool PLEN2::ExternalSD::cacheFlush(void)
{
  if (mCacheDirty) {
    if (!mSDCard.writeBlock(mCacheBlockNumber, mCacheBuf)) {
      return false;
    }
    mCacheDirty = 0;
  }
  return true;
}

bool PLEN2::ExternalSD::cacheRawBlock(uint32_t blockNumber, uint8_t action)
{
  if (mCacheBlockNumber != blockNumber) {
    if (!cacheFlush())
        return false;
    if (!mSDCard.readBlock(blockNumber, mCacheBuf))
        return false;
    mCacheBlockNumber = blockNumber;
  }
  mCacheDirty |= action;

  return true;
}

void PLEN2::ExternalSD::begin()
{
    System::debugSerial().println(F("-------------"));

    if (!mSDCard.init(SPI_HALF_SPEED, Pin::SD_CS)) {
        System::debugSerial().println(F("initialization failed. Things to check:"));
        System::debugSerial().println(F("* is a SD Card inserted?"));
        System::debugSerial().println(F("* is your wiring correct?"));
        System::debugSerial().println(F("* did you change the chipSelect pin to match your shield or module?"));
        return;
    } else {
        System::debugSerial().println(F("Wiring is correct and a SD Card is present."));
    }

    // print the type of mSDCard
    System::debugSerial().print(F("Card type: "));
    switch (mSDCard.type()) {
        case SD_CARD_TYPE_SD1:
            System::debugSerial().println(F("SD1"));
            break;
        case SD_CARD_TYPE_SD2:
            System::debugSerial().println(F("SD2"));
            break;
        case SD_CARD_TYPE_SDHC:
            System::debugSerial().println(F("SDHC"));
            break;
        default:
            System::debugSerial().println(F("Unknown"));
    }
    System::debugSerial().println(F("-------------"));
}

int8_t PLEN2::ExternalSD::readSlot(uint32_t slot, uint8_t data[], uint8_t read_size)
{
    uint32_t block;
    uint16_t offset;

    #if DEBUG
        PROFILING("ExternalSD::readSlot()");
    #endif


    if (   (slot >= SLOT_END)
        || (read_size > SLOT_SIZE)
    )
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument! : slot = "));
            System::debugSerial().print(slot);
            System::debugSerial().print(F(", or read_size = "));
            System::debugSerial().println(read_size);
        #endif

        return -1;
    }

    block  = (slot * SLOT_SIZE) / 512;
    offset = (slot * SLOT_SIZE) % 512;
    if (cacheRawBlock(block, CACHE_FOR_READ)) {
        memcpy(data, &mCacheBuf[offset], read_size);
        return read_size;
    }

    return -1;
}


int8_t PLEN2::ExternalSD::writeSlot(uint32_t slot, const uint8_t data[], uint8_t write_size)
{
    uint32_t block;
    uint16_t offset;

    #if DEBUG
        PROFILING("ExternalSD::writeSlot()");
    #endif


    if (   (slot >= SLOT_END)
        || (write_size > SLOT_SIZE)
    )
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument! : slot = "));
            System::debugSerial().print(slot);
            System::debugSerial().print(F(", or write_size = "));
            System::debugSerial().println(write_size);
        #endif

        return -1;
    }

    block  = (slot * SLOT_SIZE) / 512;
    offset = (slot * SLOT_SIZE) % 512;
    if (cacheRawBlock(block, CACHE_FOR_WRITE)) {
        memcpy(&mCacheBuf[offset], data, write_size);
        cacheFlush();
        return write_size;
    }

    return -1;
}
