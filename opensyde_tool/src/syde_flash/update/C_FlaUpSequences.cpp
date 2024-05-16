//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Integration class for basic sequence of device update.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_FlaUpSequences.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpSequences::C_FlaUpSequences(void) :
   C_OscBuSequences(),
   me_Sequence(eNOT_ACTIVE),
   mu32_FlashloaderResetWaitTime(1000),
   mc_HexFilePath(""),
   mu32_RequestDownloadTimeout(40000),
   mu32_TransferDataTimeout(1000),
   ms32_Result(C_NOACT)
{
   mpc_Thread = new C_SyvComDriverThread(&C_FlaUpSequences::mh_ThreadFunc, this);

   qRegisterMetaType<uint8_t>("uint8_t");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Destructor: clean up thread
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaUpSequences::~C_FlaUpSequences(void) noexcept
{
   if (this->mpc_Thread != NULL)
   {
      if (this->mpc_Thread->isRunning() == true)
      {
         this->mpc_Thread->requestInterruption();

         if (this->mpc_Thread->wait(2000U) == false)
         {
            // Not finished yet
            osc_write_log_warning("Closing device configuration sequences",
                                  "Waiting time for stopping thread was not enough");
         }
      }
      delete mpc_Thread;
      mpc_Thread = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start thread for flashloader activation

   \param[in]  ou32_FlashloaderResetWaitTime    Flashloader reset wait time

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaUpSequences::StartActivateFlashLoader(const uint32_t ou32_FlashloaderResetWaitTime)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eACTIVATEFLASHLOADER;
      this->mu32_FlashloaderResetWaitTime = ou32_FlashloaderResetWaitTime;
      this->mpc_Thread->start();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start thread for reading device information

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaUpSequences::StartReadDeviceInformation(void)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eREADDEVICEINFO;
      this->mpc_Thread->start();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start thread for node update

   \param[in]  orc_HexFilePath               Path to HEX file
   \param[in]  ou32_RequestDownloadTimeout   Maximum time in ms it can take to erase one continuous area in flash
   \param[in]  ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to flash

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaUpSequences::StartUpdateNode(const QString & orc_HexFilePath, const uint32_t ou32_RequestDownloadTimeout,
                                          const uint32_t ou32_TransferDataTimeout)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eUPDATENODE;
      this->mc_HexFilePath = orc_HexFilePath;
      this->mu32_RequestDownloadTimeout = ou32_RequestDownloadTimeout;
      this->mu32_TransferDataTimeout = ou32_TransferDataTimeout;
      this->mpc_Thread->start();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start thread for reset system

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaUpSequences::StartResetSystem(void)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eRESETSYSTEM;
      this->mpc_Thread->start();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result       result code of executed service function

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaUpSequences::GetResults(int32_t & ors32_Result) const
{
   int32_t s32_Return = C_NO_ERR;

   ors32_Result = C_UNKNOWN_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      ors32_Result = this->ms32_Result;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports progress in percentage

   \param[in]  ou8_ProgressInPercentage       Progress in percentage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpSequences::m_ReportProgressPercentage(const uint8_t ou8_ProgressInPercentage)
{
   Q_EMIT (this->SigReportFlashingProgress(ou8_ProgressInPercentage));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports some information about the current sequence

   Here: print to log file and signal progress log

   \param[in]  os32_Result       Result of service
   \param[in]  orc_Information   Text information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpSequences::m_ReportProgress(const int32_t os32_Result, const stw::scl::C_SclString & orc_Information)
{
   QString c_Progress;

   if (os32_Result == C_NO_ERR)
   {
      c_Progress = orc_Information.c_str();
      osc_write_log_info("Progress", c_Progress.toStdString());
   }
   else if (os32_Result == C_WARN)
   {
      c_Progress = "Warning in step: ";
      c_Progress += orc_Information.c_str();
      osc_write_log_warning("Progress", c_Progress.toStdString());
   }
   else
   {
      c_Progress =  "Error in step: ";
      c_Progress += orc_Information.c_str();
      c_Progress += " Result: " + QString::number(os32_Result) + ". See log file for details. ";
      osc_write_log_error("Progress", c_Progress.toStdString());
   }

   Q_EMIT (this->SigReportProgress(c_Progress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports information read from openSYDE flashloader of node

   Called by ReadDeviceInformation() after it has read information from an openSYDE node.

   Here: Report read information to progress log via SigReportFlashloaderInformationRead.

   \param[in]  orc_DeviceName    Device name
   \param[in]  orc_Information   Flashloader information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpSequences::m_ReportFlashloaderInformationRead(const stw::scl::C_SclString & orc_DeviceName,
                                                          const stw::opensyde_core::C_OscComFlashloaderInformation & orc_Information)
{
   const C_SclStringList c_MoreInformation = orc_Information.FlashloaderInformationToText();
   C_SclStringList c_Lines;
   QString c_ProgressText;

   c_Lines.Clear();
   c_Lines.Add("Device name: " + orc_DeviceName);
   c_Lines.AddStrings(&c_MoreInformation);

   c_ProgressText += "openSYDE device information read: \n";

   for (uint32_t u32_Line = 0U; u32_Line < c_Lines.GetCount(); u32_Line++)
   {
      c_ProgressText += "  ";
      c_ProgressText += c_Lines.Strings[u32_Line].c_str();
      if (u32_Line != (c_Lines.GetCount() - 1))
      {
         c_ProgressText += "\n";
      }
      osc_write_log_info("Flashloader Info", c_Lines.Strings[u32_Line]);
   }

   Q_EMIT (this->SigReportFlashloaderInformationText(c_ProgressText));
   Q_EMIT (this->SigReportDeviceName(orc_DeviceName.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpSequences::mh_ThreadFunc(void * const opv_Instance)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_FlaUpSequences * const pc_Sequences = reinterpret_cast<C_FlaUpSequences *>(opv_Instance);

   tgl_assert(pc_Sequences != NULL);
   if (pc_Sequences != NULL)
   {
      pc_Sequences->m_ThreadFunc();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The functions executed by the system's threading engine

   Calls the function configured via me_Sequence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaUpSequences::m_ThreadFunc(void)
{
   switch (this->me_Sequence)
   {
   case eNOT_ACTIVE:
      // Nothing to do. Should not happen.
      break;
   case eACTIVATEFLASHLOADER:
      this->ms32_Result = this->ActivateFlashLoader(mu32_FlashloaderResetWaitTime);
      break;
   case eREADDEVICEINFO:
      this->ms32_Result = this->ReadDeviceInformation();
      break;
   case eUPDATENODE:
      this->ms32_Result = this->UpdateNode(this->mc_HexFilePath.toStdString().c_str(),
                                           this->mu32_RequestDownloadTimeout, this->mu32_TransferDataTimeout);
      break;
   case eRESETSYSTEM:
      this->ms32_Result = this->ResetSystem();
      break;
   default:
      tgl_assert(false);
      break;
   }

   this->mpc_Thread->requestInterruption();
}
