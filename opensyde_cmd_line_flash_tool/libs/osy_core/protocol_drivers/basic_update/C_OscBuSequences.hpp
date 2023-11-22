//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Sequences for basic system update.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCBUSEQUENCES_HPP
#define C_OSCBUSEQUENCES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_Can.hpp"
#include "C_OscProtocolDriverOsyTpCan.hpp"
#include "C_OscProtocolDriverOsy.hpp"
#include "C_OscComFlashloaderInformation.hpp"
#include "C_OscHexFile.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscBuSequences
{
public:
   C_OscBuSequences(void);
   virtual ~C_OscBuSequences(void);
   int32_t Init(const stw::scl::C_SclString & orc_CanDllPath, const int32_t os32_CanBitrate, const uint8_t ou8_NodeId);
   int32_t ActivateFlashLoader(const uint32_t ou32_FlashloaderResetWaitTime);
   int32_t ReadDeviceInformation(void);
   int32_t UpdateNode(const stw::scl::C_SclString & orc_HexFilePath, const uint32_t ou32_RequestDownloadTimeout,
                      const uint32_t ou32_TransferDataTimeout);
   int32_t ResetSystem(void);

protected:
   virtual void m_ReportProgress(const int32_t os32_Result, const stw::scl::C_SclString & orc_Information);
   virtual void m_ReportFlashloaderInformationRead(const stw::scl::C_SclString & orc_DeviceName,
                                                   const C_OscComFlashloaderInformation & orc_Information);

private:
   // driver instances:
   stw::can::C_Can mc_CanDispatcher;
   stw::opensyde_core::C_OscProtocolDriverOsyTpCan mc_TpCan;
   stw::opensyde_core::C_OscProtocolDriverOsy mc_OsyProtocol;

   int32_t ms32_CanBitrate; // CAN bitrate in kBit/s

   static int32_t mh_ReadHexFile(const stw::scl::C_SclString & orc_HexFilePath, C_OscHexFile & orc_HexFile,
                                 uint32_t & oru32_SignatureBlockAddress);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
