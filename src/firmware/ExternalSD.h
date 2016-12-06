/*!
    @file      ExternalSD.h
    @brief     Management class of external EEPROM.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_EXTERNAL_SD_H
#define PLEN2_EXTERNAL_SD_H


#include <stdint.h>

namespace PLEN2
{
    class ExternalSD;
}

/*!
    @brief Management class of external EEPROM

    @note
    24FC1025 supports the access to 128 bytes at once, but Arduino's I2C library doesn't support it
    because the library's buffer size is 32 bytes.
    <br><br>
    Please pay attention to the fact that it is including bytes of targeted area address (= 2 bytes).
    The accurate data size that you can write is 30 bytes.
    (This is why there are differences between CHUNK_SIZE and SLOT_SIZE.)
*/
class PLEN2::ExternalSD
{
private:
    //! @brief Size of external EEPROM (bytes)
    enum { SIZE = 0x20000UL };

    //! @brief Bytes of targeted area address
    enum { ADDRESS_BYTES = 2 };

    static bool cacheFlush(void);
    static bool cacheRawBlock(uint32_t blockNumber, uint8_t action);


public:
    //! @brief Chunk size of external EEPROM (bytes)
    enum { CHUNK_SIZE = 32 };

    //! @brief Slot size of external EEPROM (bytes)
    enum { SLOT_SIZE = CHUNK_SIZE - ADDRESS_BYTES };

    //! @brief Beginning value of slots
    enum { SLOT_BEGIN = 0 };

    //! @brief End value of slots
    enum { SLOT_END = SIZE / CHUNK_SIZE };

    /*!
        @brief Constructor
    */
    ExternalSD() { begin(); }

    /*!
        @brief Static constructor
    */
    static void begin();

    /*!
        @brief Read a slot of external EEPROM

        @param [in]  slot      Please set slot number you want to read.
        @param [out] data[]    Please set buffer to store reading data.
        @param [in]  read_size Please set buffer size.

        @return Result
        @retval !0 Succeeded. (Generally, the value equals **read-size**.)
        @retval -1 Failed.
    */
    static int8_t readSlot(uint32_t slot, uint8_t data[], uint8_t read_size);

    /*!
        @brief Write a slot of external EEPROM

        @param [in] slot       Please set slot number you want to write.
        @param [in] data[]     Please set buffer that stored writing data.
        @param [in] write_size Please set buffer size.

        @return Result
        @retval 0  Succeeded.
        @retval -1 Argument error. (**write_size** is bigger than slot size.)
        @retval 1  Sending-buffer overflow.
        @retval 2  Received NACK after sending slave address.
        @retval 3  Received NACK after sending data bytes.
        @retval 4  Other errors were raised.

        @attention
        Writing external EEPROM requires time. (Typically using 3[msec].)
        In the implementation, 5[msec] delay is inserted at the end of the method.
    */
    static int8_t writeSlot(uint32_t slot, const uint8_t data[], uint8_t write_size);
};

#endif // PLEN2_EXTERNAL_SD_H
