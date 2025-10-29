//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Integration class for sequence of device configuration, containing search nodes and configure node.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_FlaSenDcBasicSequences.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
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
C_FlaSenDcBasicSequences::C_FlaSenDcBasicSequences(void) :
   QObject(),
   C_OscDcBasicSequences(),
   me_Sequence(eNOT_ACTIVE),
   mu32_FlashloaderResetWaitTime(1000),
   mu8_CurrentNodeId(0),
   mu8_NewNodeId(0),
   mu32_CanBitrate(125),
   mu8_InterfaceIndex(0),
   ms32_Result(C_NOACT)
{
   mpc_Thread = new C_SyvComDriverThread(&C_FlaSenDcBasicSequences::mh_ThreadFunc, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Destructor: clean up thread
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSenDcBasicSequences::~C_FlaSenDcBasicSequences(void) noexcept
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

   this->PrepareForDestruction();
   try
   {
      if (mc_CanDispatcher.DLL_Close() == C_NO_ERR)
      {
         osc_write_log_info("Teardown", "CAN DLL closed.");
      }
      else
      {
         osc_write_log_info("Teardown", "Failed to close CAN DLL.");
      }
   }
   catch (...)
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize transport protocol, openSYDE protocol driver and CAN with given parameters.

   \param[in]  orc_CanDllPath    Path to CAN DLL file
   \param[in]  os32_CanBitrate   CAN Bitrate in kBit/s

   \return
   C_NO_ERR    everything ok
   else        error occurred, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSenDcBasicSequences::InitDcSequences(const C_SclString & orc_CanDllPath, const int32_t os32_CanBitrate)
{
   int32_t s32_Return;
   const C_SclString c_LogActivity = "Initialization";

   osc_write_log_info(c_LogActivity, "CAN DLL path used: " + orc_CanDllPath);

   mc_CanDispatcher.SetDLLName(orc_CanDllPath);
   s32_Return = mc_CanDispatcher.DLL_Open();
   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info(c_LogActivity, "CAN DLL loaded.");
      s32_Return = mc_CanDispatcher.CAN_Init(os32_CanBitrate);
      if (s32_Return == C_NO_ERR)
      {
         osc_write_log_info(c_LogActivity, "CAN interface initialized.");
      }
      else
      {
         osc_write_log_error(c_LogActivity, "Could not initialize the CAN interface!");
      }
   }
   else
   {
      const std::string c_Bitness = QString::number(8 * sizeof(size_t)).toStdString();
      osc_write_log_error(c_LogActivity,
                          "Could not load the CAN DLL! Make sure to use a " + c_Bitness + "-bit DLL.");
   }

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = this->Init(&this->mc_CanDispatcher);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start thread for scan enter flashloader

   \param[in]  ou32_FlashloaderResetWaitTime    Flashloader reset wait time

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSenDcBasicSequences::StartScanEnterFlashloader(const uint32_t ou32_FlashloaderResetWaitTime)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eSCAN_ENTER_FLASHLOADER;
      this->mu32_FlashloaderResetWaitTime = ou32_FlashloaderResetWaitTime;
      this->mpc_Thread->start();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start thread for scan get info

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSenDcBasicSequences::StartScanGetInfo(void)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eSCAN_GET_INFO;
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
int32_t C_FlaSenDcBasicSequences::StartResetSystem(void)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eRESET_SYSTEM;
      this->mpc_Thread->start();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start thread for configure device

   \param[in]  ou8_CurrentNodeId    ID of Node we want to configure
   \param[in]  ou8_NewNodeId        Node ID about to configure
   \param[in]  ou32_Bitrate         Bitrate about to configure
   \param[in]  ou8_InterfaceIndex   Interface that is about to be configured

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_FlaSenDcBasicSequences::StartConfigureDevice(const uint8_t ou8_CurrentNodeId, const uint8_t ou8_NewNodeId,
                                                       const uint32_t ou32_Bitrate, const uint8_t ou8_InterfaceIndex)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eCONF_DEVICES;

      this->mu8_CurrentNodeId = ou8_CurrentNodeId;
      this->mu8_NewNodeId = ou8_NewNodeId;
      this->mu32_CanBitrate = ou32_Bitrate;
      this->mu8_InterfaceIndex = ou8_InterfaceIndex;
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
int32_t C_FlaSenDcBasicSequences::GetResults(int32_t & ors32_Result) const
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
/*! \brief  Returns current sequence
   \return
    current sequence that is running
*/
//----------------------------------------------------------------------------------------------------------------------
C_FlaSenDcBasicSequences::E_Sequence C_FlaSenDcBasicSequences::GetCurrentSequence() const
{
   return this->me_Sequence;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports some information about the current sequence

   Here: print to log file and signal progress log

   \param[in]  os32_Result       Result of service
   \param[in]  orc_Information   Text information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenDcBasicSequences::m_ReportProgress(const int32_t os32_Result,
                                                const stw::scl::C_SclString & orc_Information)
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
      c_Progress += " Result: " + QString::number(os32_Result) + ".";
      osc_write_log_error("Progress", c_Progress.toStdString());
      // don't write log file hint into log file, just append afterwards:
      c_Progress += " See log file for details.";
      C_OscLoggingHandler::h_Flush();
   }

   Q_EMIT (this->SigReportProgress(c_Progress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports device information read from found devices

   Called by ScanGetInfo() after it has scanned information from connected devices.

   Here: Report read information to dialog.

   \param[in]  orc_DeviceInfoResult    Device information results
   \param[in]  oq_SecurityFeatureUsed  Security feature used for at least one node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenDcBasicSequences::m_ReportDevicesInfoRead(
   const std::vector<stw::opensyde_core::C_OscDcDeviceInformation> & orc_DeviceInfoResult,
   const bool oq_SecurityFeatureUsed)
{
   const QString c_Message =
      C_OscDcBasicSequences::h_DevicesInfoToString(orc_DeviceInfoResult, oq_SecurityFeatureUsed).c_str();

   if (c_Message == "Found 0 device(s):")
   {
      osc_write_log_warning("Report", c_Message.toStdString().c_str());
   }
   else
   {
      osc_write_log_info("Report", c_Message.toStdString().c_str());
   }

   Q_EMIT (this->SigReportDevicesInfoRead(orc_DeviceInfoResult, oq_SecurityFeatureUsed));
}

//----------------------------------------------------------------------------------------------------------------------
void C_FlaSenDcBasicSequences::mh_ThreadFunc(void * const opv_Instance)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_FlaSenDcBasicSequences * const pc_Sequences = reinterpret_cast<C_FlaSenDcBasicSequences *>(opv_Instance);

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
void C_FlaSenDcBasicSequences::m_ThreadFunc(void)
{
   switch (this->me_Sequence)
   {
   case eNOT_ACTIVE:
      // Nothing to do. Should not happen.
      break;
   case eSCAN_ENTER_FLASHLOADER:
      this->ms32_Result = this->ScanEnterFlashloader(mu32_FlashloaderResetWaitTime);
      break;
   case eSCAN_GET_INFO:
      this->ms32_Result = this->ScanGetInfo();
      break;
   case eRESET_SYSTEM:
      this->ms32_Result = this->ResetSystem();
      break;
   case eCONF_DEVICES:
      this->ms32_Result = this->ConfigureDevice(mu8_CurrentNodeId, mu8_NewNodeId, mu32_CanBitrate, mu8_InterfaceIndex);
      break;
   default:
      tgl_assert(false);
      break;
   }

   this->mpc_Thread->requestInterruption();
}
