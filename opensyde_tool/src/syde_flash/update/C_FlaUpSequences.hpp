//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Integration class for basic sequence of device update.
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAUPSEQUENCES_HPP
#define C_FLAUPSEQUENCES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "C_OscBuSequences.hpp"
#include "C_SyvComDriverThread.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaUpSequences :
   public QObject,
   public stw::opensyde_core::C_OscBuSequences
{
   Q_OBJECT

public:
   C_FlaUpSequences(void);
   ~C_FlaUpSequences(void) noexcept override;

   int32_t StartActivateFlashLoader(const uint32_t ou32_FlashloaderResetWaitTime);
   int32_t StartReadDeviceInformation(void);
   int32_t StartUpdateNode(const QString & orc_HexFilePath, const uint32_t ou32_RequestDownloadTimeout,
                           const uint32_t ou32_TransferDataTimeout);
   int32_t StartResetSystem(void);

   int32_t GetResults(int32_t & ors32_Result) const;

   enum E_Sequence
   {
      eNOT_ACTIVE,
      eACTIVATEFLASHLOADER,
      eREADDEVICEINFO,
      eUPDATENODE,
      eRESETSYSTEM
   };

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigReportProgress(const QString & orc_Information);
   void SigReportFlashingProgress(const uint8_t ou8_Progress);
   void SigReportFlashloaderInformationText(const QString & orc_Information);
   void SigReportDeviceName(const QString & orc_DeviceName);

protected:
   void m_ReportProgressPercentage(const uint8_t ou8_ProgressInPercentage) override;
   void m_ReportProgress(const int32_t os32_Result, const stw::scl::C_SclString & orc_Information) override;
   void m_ReportFlashloaderInformationRead(const stw::scl::C_SclString & orc_DeviceName,
                                           const stw::opensyde_core::C_OscComFlashloaderInformation & orc_Information)
   override;

private:
   //Avoid call
   C_FlaUpSequences(const C_FlaUpSequences &);
   C_FlaUpSequences & operator =(const C_FlaUpSequences &) &; //lint !e1511 //we want to hide the base func.

   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   C_SyvComDriverThread * mpc_Thread;

   // Sequence execution parameter
   E_Sequence me_Sequence;

   // Input parameter for sequence
   uint32_t mu32_FlashloaderResetWaitTime;
   QString mc_HexFilePath;
   uint32_t mu32_RequestDownloadTimeout;
   uint32_t mu32_TransferDataTimeout;

   // Service execution result
   int32_t ms32_Result;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
