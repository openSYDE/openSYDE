//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Integration class for sequence of device configuration, containing search nodes and configure node.
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLASENDCBASICSEQUENCES_HPP
#define C_FLASENDCBASICSEQUENCES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "C_OscDcBasicSequences.hpp"
#include "C_SyvComDriverThread.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaSenDcBasicSequences :
   public QObject,
   public stw::opensyde_core::C_OscDcBasicSequences
{
   Q_OBJECT

public:
   C_FlaSenDcBasicSequences(void);
   ~C_FlaSenDcBasicSequences(void) noexcept override;

   int32_t StartScanEnterFlashloader(const uint32_t ou32_FlashloaderResetWaitTime);
   int32_t StartScanGetInfo(void);
   int32_t StartResetSystem(void);
   int32_t StartConfigureDevice(const uint8_t ou8_CurrentNodeId, const uint8_t ou8_NewNodeId,
                                const uint32_t ou32_Bitrate, const uint8_t ou8_InterfaceIndex);

   int32_t GetResults(int32_t & ors32_Result) const;

   enum E_Sequence
   {
      eNOT_ACTIVE,
      eSCAN_ENTER_FLASHLOADER,
      eSCAN_GET_INFO,
      eRESET_SYSTEM,
      eCONF_DEVICES
   };

   E_Sequence GetCurrentSequence(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigReportProgress(const QString & orc_Information);
   void SigReportDevicesInfoRead(const std::vector<stw::opensyde_core::C_OscDcDeviceInformation> & orc_DeviceInfoResult,
                                 const bool oq_SecurityFeatureUsed);

protected:
   void m_ReportProgress(const int32_t os32_Result, const stw::scl::C_SclString & orc_Information) override;
   void m_ReportDevicesInfoRead(const std::vector<stw::opensyde_core::C_OscDcDeviceInformation> & orc_DeviceInfoResult,
                                const bool oq_SecurityFeatureUsed) override;

private:
   //Avoid call
   C_FlaSenDcBasicSequences(const C_FlaSenDcBasicSequences &);
   C_FlaSenDcBasicSequences & operator =(const C_FlaSenDcBasicSequences &) &; //lint !e1511 //we want to hide the base
                                                                              // func.

   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   C_SyvComDriverThread * mpc_Thread;

   // Sequence execution parameter
   E_Sequence me_Sequence;

   // Input parameter for sequence
   uint32_t mu32_FlashloaderResetWaitTime;
   uint8_t mu8_CurrentNodeId;
   uint8_t mu8_NewNodeId;
   uint32_t mu32_CanBitrate;
   uint8_t mu8_InterfaceIndex;
   // Service execution result
   int32_t ms32_Result;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
