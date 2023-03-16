//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       GUI Integration class for system update sequences

   Strategy for GUI interaction:
   All functions are executed in a thread.
   The function call just starts the process and returns. The caller is responsible to
   keep the specified "C_SyvComDriverFlash" stable till the thread was finished.

   Intermediate results are signaled to the GUI via Qt signals. A signal is also triggered when
   the functions finish. The results are passed along with the signals.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "TglTime.hpp"
#include "C_Uti.hpp"
#include "C_GtGetText.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SyvUpSequences.hpp"
#include "C_SyvComDriverUtil.hpp"
#include "DLLocalize.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpSequences::C_SyvUpSequences(void) :
   C_OscSuSequences(),
   mpc_CanDllDispatcher(NULL),
   mpc_EthernetDispatcher(NULL),
   mq_AbortFlag(false),
   mu32_ViewIndex(0U),
   me_Sequence(eNOT_ACTIVE),
   ms32_Result(C_NOACT)
{
   mpc_Lock = new stw::tgl::C_TglCriticalSection();
   mpc_Thread = new C_SyvComDriverThread(&C_SyvUpSequences::mh_ThreadFunc, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpSequences::~C_SyvUpSequences(void)
{
   if (this->mpc_Thread != NULL)
   {
      if (this->mpc_Thread->isRunning() == true)
      {
         this->mpc_Thread->requestInterruption();

         if (this->mpc_Thread->wait(2000U) == false)
         {
            // Not finished yet
            osc_write_log_warning("Closing update sequences",
                                  "Waiting time for stopping thread was not enough");
         }
      }
      delete mpc_Thread;
      mpc_Thread = NULL;
   }

   if (this->mpc_ComDriver != NULL)
   {
      this->mpc_ComDriver->PrepareForDestructionFlash();
   }

   delete mpc_Lock;

   if (this->mpc_CanDllDispatcher != NULL)
   {
      this->mpc_CanDllDispatcher->CAN_Exit();
      this->mpc_CanDllDispatcher->DLL_Close();
      delete mpc_CanDllDispatcher;
   }

   delete mpc_EthernetDispatcher;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all members

   \param[in]  ou32_ViewIndex    Index of current used view

   \return
   C_NO_ERR      Operation success
   C_NOACT       Parameter ou32_ViewIndex invalid or no active nodes
   C_CONFIG      Invalid system definition/view configuration
   C_RD_WR       Configured communication DLL does not exist
   C_OVERFLOW    Unknown transport protocol or unknown diagnostic server for at least one node
   C_BUSY        System view error detected
   C_COM         CAN initialization failed or no route found for at least one node
   C_CHECKSUM    Internal buffer overflow detected
   C_RANGE       Routing configuration failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::InitUpSequences(const uint32_t ou32_ViewIndex)
{
   int32_t s32_Return;
   uint32_t u32_ActiveBusIndex;

   std::vector<uint8_t> c_ActiveNodes;

   this->mu32_ViewIndex = ou32_ViewIndex;

   s32_Return = C_SyvComDriverUtil::h_GetOscComDriverParamFromView(ou32_ViewIndex, u32_ActiveBusIndex,
                                                                   c_ActiveNodes,
                                                                   &this->mpc_CanDllDispatcher,
                                                                   &this->mpc_EthernetDispatcher, true, false, NULL);

   if (s32_Return == C_NO_ERR)
   {
      // pem folder is optional -> no error handling
      mc_PemDatabase.ParseFolder(C_Uti::h_GetPemDbPath().toStdString());

      s32_Return = C_OscComSequencesBase::Init(C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinition(),
                                               u32_ActiveBusIndex, c_ActiveNodes, this->mpc_CanDllDispatcher,
                                               this->mpc_EthernetDispatcher, &this->mc_PemDatabase);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reinitialize the CAN dispatcher

   The instance must be initialized with InitUpSequences before.
   InitUpSequences will initialize the dispatcher too.
   The function can initialize the dispatcher again after calling CloseDispatcher.

   \return
   C_NO_ERR    CAN dispatcher initialized
   C_CONFIG    View is invalid or initialization was not finished
   C_COM       CAN initialization failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::ReinitDispatcher(void)
{
   int32_t s32_Return = C_CONFIG;

   if (this->IsInitialized() == true)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_OscSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOscBus(pc_View->GetOscPcData().GetBusIndex());

         if (pc_Bus != NULL)
         {
            if (pc_Bus->e_Type == C_OscSystemBus::eCAN)
            {
               if (this->mpc_CanDllDispatcher != NULL)
               {
                  s32_Return = this->mpc_CanDllDispatcher->CAN_Init(
                     static_cast<int32_t>(pc_Bus->u64_BitRate / 1000ULL));

                  if (s32_Return != C_NO_ERR)
                  {
                     s32_Return = C_COM;
                  }
               }
               else
               {
                  s32_Return = C_COM;
               }
            }
            else
            {
               // Ethernet, no initialization necessary
               s32_Return = C_NO_ERR;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Close the CAN dispatcher without cleaning up the instance

   It can be reopened with ReinitDispatcher
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSequences::CloseDispatcher(void)
{
   if ((this->IsInitialized() == true) &&
       (this->mpc_CanDllDispatcher != NULL))
   {
      this->mpc_CanDllDispatcher->CAN_Exit();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creating temporary folder

   \param[in]     orc_TargetPath          Path for temporary folder
   \param[in,out] orc_ApplicationsToWrite Vector with all file paths which will be adapted as output
   \param[out]    orc_ErrorPath           Optional pointer to store path error details (current: which file did fail)

   \return
   C_NO_ERR    Temporary folder created
   C_CONFIG    View is invalid or initialization was not finished
   C_OVERFLOW  size of orc_ApplicationsToWrite is not the same as the size of nodes in orc_Nodes
               size of orc_ActiveNodes is not the same as the size of nodes in orc_Nodes
   C_NOACT     orc_ApplicationsToWrite has non-empty list of files for node that was not set as active in orc_ActiveNodes
               size of files in orc_ApplicationsToWrite[node] is higher than the
                 number of applications of the node in orc_Nodes (for an active and address based node)
   C_RANGE     file referenced by orc_ApplicationsToWrite does not exist
               orc_TargetPath does not end in "\" or "/"
   C_BUSY      could not erase pre-existing target path (note: can result in partially erased target path)
   C_RD_WR     could not copy file
   C_TIMEOUT   could not create target directory
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::SyvUpCreateTemporaryFolder(const C_SclString & orc_TargetPath,
                                                     std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                                     QString & orc_ErrorPath) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   int32_t s32_Return = C_CONFIG;

   if ((this->IsInitialized() == true) &&
       (pc_View != NULL))
   {
      C_SclString c_ErrorPath;
      s32_Return = C_OscSuSequences::h_CreateTemporaryFolder(
         C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().c_Nodes,
         this->mc_ActiveNodes,
         orc_TargetPath,
         orc_ApplicationsToWrite, &c_ErrorPath);
      orc_ErrorPath = c_ErrorPath.c_str();
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get result of previously started service execution

   Can be used to extract the results of one service execution after it has finished.

   \param[out]  ors32_Result       result code of executed service function
                                   for possible values see the DataDealer's function documentation

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::GetResults(int32_t & ors32_Result) const
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
/*! \brief   Returns the last position in the update sequence

   Which file of which node was the last update object

   \param[out]    oru32_NodeIndex       Index of node in system definition
   \param[out]    oru32_FileIndex       Index of file

   \return
   C_NO_ERR       result code read
   C_BUSY         previously started polled communication still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::GetLastUpdatePosition(uint32_t & oru32_NodeIndex, uint32_t & oru32_FileIndex) const
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      oru32_NodeIndex = this->mu32_CurrentNode;
      oru32_FileIndex = this->mu32_CurrentFile;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the reported results of StartReadDeviceInformation for openSYDE nodes

   If the both vectors have a different size an tgl assert will be reported.
   After reading the information, the vectors will be cleared.

   \param[out]    orc_OsyNodeIndexes         All node indexes
   \param[out]    orc_OsyDeviceInformation   All device information associated to the node index in the same order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSequences::GetOsyDeviceInformation(std::vector<uint32_t> & orc_OsyNodeIndexes,
                                               std::vector<C_OscSuSequences::C_OsyDeviceInformation> & orc_OsyDeviceInformation)
{
   this->mpc_Lock->Acquire();

   tgl_assert(this->mc_ReportOsyDeviceInformationNodeIndex.size() == this->mc_ReportOsyDeviceInformation.size());

   // Copy all elements
   orc_OsyNodeIndexes = this->mc_ReportOsyDeviceInformationNodeIndex;
   orc_OsyDeviceInformation = this->mc_ReportOsyDeviceInformation;
   this->mc_ReportOsyDeviceInformationNodeIndex.clear();
   this->mc_ReportOsyDeviceInformation.clear();

   this->mpc_Lock->Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the reported results of StartReadDeviceInformation for STW Flashloader nodes

   If the both vectors have a different size an tgl assert will be reported.
   After reading the information, the vectors will be cleared.

   \param[out]    orc_XflNodeIndexes         All node indexes
   \param[out]    orc_XflDeviceInformation   All device information associated to the node index in the same order
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSequences::GetXflDeviceInformation(std::vector<uint32_t> & orc_XflNodeIndexes,
                                               std::vector<C_OscSuSequences::C_XflDeviceInformation> & orc_XflDeviceInformation)
{
   this->mpc_Lock->Acquire();

   tgl_assert(this->mc_ReportXflDeviceInformationNodeIndex.size() == this->mc_ReportXflDeviceInformation.size());

   // Copy all elements
   orc_XflNodeIndexes = this->mc_ReportXflDeviceInformationNodeIndex;
   orc_XflDeviceInformation = this->mc_ReportXflDeviceInformation;
   this->mc_ReportXflDeviceInformationNodeIndex.clear();
   this->mc_ReportXflDeviceInformation.clear();

   this->mpc_Lock->Release();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the name of the current step

   \param[in]     oe_Step         Step of node configuration

   \return
   Name of the specific step
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpSequences::GetStepName(const E_ProgressStep oe_Step) const
{
   QString c_Text;

   switch (oe_Step)
   {
   case eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_START:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Request programming start");
      break;
   case eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_WARNING:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Request programming warning");
      break;
   case eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Request programming error");
      break;
   case eACTIVATE_FLASHLOADER_OSY_ECU_RESET_WARNING:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: ECU reset warning");
      break;
   case eACTIVATE_FLASHLOADER_OSY_ECU_RESET_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: ECU reset error");
      break;
   case eACTIVATE_FLASHLOADER_XFL_ECU_RESET_WARNING:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: No ECU reset message configured");
      break;
   case eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Sending ECU reset request error");
      break;
   case eACTIVATE_FLASHLOADER_OSY_XFL_BC_ENTER_FLASHLOADER_START:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Broadcast enter Flashloader start");
      break;
   case eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Broadcast enter pre programming error");
      break;
   case eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Broadcast \"FLASH\" error");
      break;
   case eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Ping devices start");
      break;
   case eACTIVATE_FLASHLOADER_OSY_RECONNECT_WARNING:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Reconnect warning");
      break;
   case eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Reconnect error");
      break;
   case eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Set session error");
      break;
   case eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Perform wakeup error");
      break;
   case eACTIVATE_FLASHLOADER_ROUTING_START:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Start routing");
      break;
   case eACTIVATE_FLASHLOADER_ROUTING_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Error on start routing");
      break;
   case eACTIVATE_FLASHLOADER_ROUTING_AVAILABLE_FEATURE_ERROR:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Error on start routing due to available features");
      break;
   case eACTIVATE_FLASHLOADER_FINISHED:
      c_Text = C_GtGetText::h_GetText("Activate Flashloader: Finished");
      break;
   case eREAD_DEVICE_INFO_START:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Start");
      break;
   case eREAD_DEVICE_INFO_OSY_RECONNECT_ERROR:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Error on reconnecting");
      break;
   case eREAD_DEVICE_INFO_OSY_SET_SESSION_START:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Set session start");
      break;
   case eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Set session error");
      break;
   case eREAD_DEVICE_INFO_OSY_DEVICE_NAME_START:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Device name start");
      break;
   case eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Device name error");
      break;
   case eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_START:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Flash blocks start");
      break;
   case eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Flash blocks security error");
      break;
   case eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Flash blocks error");
      break;
   case eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_START:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Flashloader information start");
      break;
   case eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Flashloader information error");
      break;
   case eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_START:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Flashloader security activation state start");
      break;
   case eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_ERROR:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Flashloader security activation state error");
      break;
   case eREAD_DEVICE_INFO_FINISHED:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Finished");
      break;
   case eREAD_DEVICE_INFO_OSY_START:
      c_Text = C_GtGetText::h_GetText("Read openSYDE Device Information: Started");
      break;
   case eREAD_DEVICE_INFO_OSY_FINISHED:
      c_Text = C_GtGetText::h_GetText("Read openSYDE Device Information: Finished");
      break;
   case eREAD_DEVICE_INFO_XFL_START:
      c_Text = C_GtGetText::h_GetText("Read STW Flashloader Device Information: Started");
      break;
   case eREAD_DEVICE_INFO_XFL_FINISHED:
      c_Text = C_GtGetText::h_GetText("Read STW Flashloader Device Information: Finished");
      break;
   case eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Connection with STW Flashloader device failed");
      break;
   case eREAD_DEVICE_INFO_XFL_READING_INFORMATION_START:
      c_Text = C_GtGetText::h_GetText("Read Device Information: Start");
      break;
   case eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR:
      c_Text =
         C_GtGetText::h_GetText("Read Device Information: Could not read information from STW Flashloader device");
      break;
   case eUPDATE_SYSTEM_START:
      c_Text = C_GtGetText::h_GetText("Update System: Start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_READ_FEATURE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node read of available features error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node finished");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node open HEX file start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node open HEX file error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node HEX file signature error");
      break;
   case eUPDATE_SYSTEM_OSY_RECONNECT_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Error on reconnecting");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node check device name start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node check device name communication error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node check device name file error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node check device name match error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node check memory start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node check memory session error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node check memory file error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK:
      c_Text = C_GtGetText::h_GetText("Update System: Node check memory not OK");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node fingerprint start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE:
      c_Text = C_GtGetText::h_GetText("Update System: Node fingerprint name is not readable");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node fingerprint error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of HEX file start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of HEX file start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of HEX file erase error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of HEX file transfer start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of HEX file transfer error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of HEX file exit start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_FINAL_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of HEX file exit final start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of HEX file exit error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of HEX file finished");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of file system file start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Preparation of Node flash area of file system file start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Preparation of Node flash area of file system file erase error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of file system file transfer start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash area of file system file transfer error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of file system file exit start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of file system file exit error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_RESULT_STRING:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of file system file read exit result");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of file system file finished");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_AVAILABLE_FEATURE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write available features error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_SESSION_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write session error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_MAX_SIZE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write read back of maximum block length error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write read back of parameter set image file start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write read back of parameter set image file error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write write of parameter set image file start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write write of parameter set image file error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FILE_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write of parameter set image file finished");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node NVM write of parameter set image files finished");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of PEM file start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_AVAILABLE_FEATURE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of PEM file available feature error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SESSION_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of PEM file session error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_OPEN_FILE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of PEM file open file error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_EXTRACT_KEY_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of PEM file extract key error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SEND_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of PEM file send error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of PEM file finished");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of security activation state start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_AVAILABLE_FEATURE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of security activation state available feature error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SESSION_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of security activation state session error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SEND_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of security activation state send error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of security activation state finished");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of debugger activation state start");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_AVAILABLE_FEATURE_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of debugger activation state available feature error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SESSION_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of debugger activation state session error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SEND_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of debugger activation state send error");
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node write of debugger activation state finished");
      break;
   case eUPDATE_SYSTEM_ABORTED:
      c_Text = C_GtGetText::h_GetText("Update System: Aborted");
      break;
   case eUPDATE_SYSTEM_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Finished");
      break;
   case eUPDATE_SYSTEM_XFL_NODE_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash start");
      break;
   case eUPDATE_SYSTEM_XFL_NODE_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash finished");
      break;
   case eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of HEX file start");
      break;
   case eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of HEX file error");
      break;
   case eXFL_PROGRESS: //wrapped progress information from STW flashloader driver
      c_Text = C_GtGetText::h_GetText("Update System: STW Flashloader status");
      break;
   case eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED:
      c_Text = C_GtGetText::h_GetText("Update System: Node flash of HEX file finished");
      break;
   case eRESET_SYSTEM_START:
      c_Text = C_GtGetText::h_GetText("Reset System: Start");
      break;
   case eRESET_SYSTEM_OSY_NODE_ERROR:
      c_Text = C_GtGetText::h_GetText("Reset System: Node error");
      break;
   case eRESET_SYSTEM_OSY_ROUTED_NODE_ERROR:
      c_Text = C_GtGetText::h_GetText("Reset System: Routed node error");
      break;
   case eRESET_SYSTEM_FINISHED:
      c_Text = C_GtGetText::h_GetText("Reset System: Finished");
      break;
   default:
      tgl_assert(false);
      break;
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to activate flashloader

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::StartActivateFlashloader(void)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mq_AbortFlag = false;

      this->me_Sequence = eACTIVATE_FLASHLOADER;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to read device information

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::StartReadDeviceInformation(void)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->mc_ReportOsyDeviceInformationNodeIndex.clear();
      this->mc_ReportOsyDeviceInformation.clear();
      this->mc_ReportXflDeviceInformationNodeIndex.clear();
      this->mc_ReportXflDeviceInformation.clear();
      this->mq_AbortFlag = false;

      this->me_Sequence = eREAD_DEVICE_INFORMATION;
      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to update system

   \param[in]  orc_ApplicationsToWrite   list of files to flash per node; must have the same size as the system definition
                                   contains nodes
   \param[in]  orc_NodesOrder     Vector with node update order (index is update position, value is node index)

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::StartUpdateSystem(const std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                                            const std::vector<uint32_t> & orc_NodesOrder)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eUPDATE_SYSTEM;
      this->mc_NodesToFlash.clear();
      this->mc_NodesToFlash = orc_ApplicationsToWrite;
      this->mc_NodesOrder.clear();
      this->mc_NodesOrder = orc_NodesOrder;
      this->mq_AbortFlag = false;

      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts thread to reset system

   \return
   C_NO_ERR   started sequence
   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::StartResetSystem(void)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      this->me_Sequence = eRESET_SYSTEM;
      this->mq_AbortFlag = false;

      this->mpc_Thread->start();
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Aborts the current sequence

   No problem with thread safety because of a bool value.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSequences::AbortCurrentProgress(void)
{
   this->mq_AbortFlag = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the node states of the connect process

   The connect process involves the both steps:
   * ActivateFlashloader
   * ReadDeviceInformation

   \param[out]      orc_ConnectStatesNodes   Detailed output parameter description

   \retval   C_NO_ERR   States returned
   \retval   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::GetConnectStates(std::vector<C_OscSuSequencesNodeConnectStates> & orc_ConnectStatesNodes)
const
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      s32_Return = C_OscSuSequences::GetConnectStates(orc_ConnectStatesNodes);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the node states of the connect process

   The connect process involves the both steps:
   * ActivateFlashloader
   * ReadDeviceInformation

   \param[out]      orc_UpdateStatesNodes   Detailed output parameter description

   \retval   C_NO_ERR   States returned
   \retval   C_BUSY     previously started sequence still going on
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpSequences::GetUpdateStates(std::vector<C_OscSuSequencesNodeUpdateStates> & orc_UpdateStatesNodes) const
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Thread->isRunning() == true)
   {
      s32_Return = C_BUSY;
   }
   else
   {
      s32_Return = C_OscSuSequences::GetUpdateStates(orc_UpdateStatesNodes);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports some information about the current sequence

   Give textual information to logging engine

   \param[in]     oe_Step           Step of node update
   \param[in]     os32_Result       Result of service
   \param[in]     ou8_Progress      Progress of sequence in percentage (goes from 0..100 for each function)
   \param[in]     orc_Information   Additional text information

   \return
   Flag for aborting sequence
   - true   abort sequence
   - false  continue sequence
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpSequences::m_ReportProgress(const E_ProgressStep oe_Step, const int32_t os32_Result,
                                        const uint8_t ou8_Progress, const C_SclString & orc_Information)
{
   C_SclString c_Text;

   if ((oe_Step != C_SyvUpSequences::eXFL_PROGRESS) ||
       (orc_Information.Pos(TGL_LoadStr(STR_FDL_TXT_WR_FLASH_RQ)) == 0))
   {
      c_Text =  ("Step: " + this->GetStepName(oe_Step)).toStdString().c_str();
      c_Text += " Progress: " + C_SclString::IntToStr(ou8_Progress);
      c_Text += " Result: " + C_SclString::IntToStr(os32_Result);
      c_Text += " Info: " + orc_Information;

      mh_WriteLog(oe_Step, c_Text);
   }

   // Enum make problems with signal slot mechanism
   Q_EMIT this->SigReportProgress(static_cast<uint32_t>(oe_Step), os32_Result, ou8_Progress);

   return this->mq_AbortFlag;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports some information about the current sequence for a specific server

   \param[in]     oe_Step           Step of node update
   \param[in]     os32_Result       Result of service
   \param[in]     ou8_Progress      Progress of sequence in percentage (goes from 0..100 for each function)
                                    Progress invalid: 255
   \param[in]     orc_Server        Affected node
   \param[in]     orc_Information   Additional text information

   \return
   Flag for aborting sequence
   - true   abort sequence
   - false  continue sequence
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpSequences::m_ReportProgress(const E_ProgressStep oe_Step, const int32_t os32_Result,
                                        const uint8_t ou8_Progress, const C_OscProtocolDriverOsyNode & orc_Server,
                                        const C_SclString & orc_Information)
{
   C_SclString c_Text;

   if ((oe_Step != C_SyvUpSequences::eXFL_PROGRESS) ||
       (orc_Information.Pos(TGL_LoadStr(STR_FDL_TXT_WR_FLASH_RQ)) == 0))
   {
      c_Text =  ("Step: " + this->GetStepName(oe_Step)).toStdString().c_str();
      c_Text += " Progress: " + C_SclString::IntToStr(ou8_Progress);
      c_Text += " Result: " + C_SclString::IntToStr(os32_Result);
      c_Text += " Bus ID: " + C_SclString::IntToStr(orc_Server.u8_BusIdentifier);
      c_Text += " Node ID: " + C_SclString::IntToStr(orc_Server.u8_NodeIdentifier);
      c_Text += " Info: " + orc_Information;

      mh_WriteLog(oe_Step, c_Text);
   }

   // Enum make problems with signal slot mechanism
   Q_EMIT this->SigReportProgressForServer(static_cast<uint32_t>(oe_Step), os32_Result, ou8_Progress,
                                           orc_Server.u8_BusIdentifier,
                                           orc_Server.u8_NodeIdentifier);

   return this->mq_AbortFlag;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports information read from openSYDE server node

   Called by ReadDeviceInformation() after it has read information from an openSYDE node.
   Here: save device information

   \param[in]     orc_Info          Information read from node
   \param[in]     ou32_NodeIndex   Index of node within mpc_SystemDefinition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSequences::m_ReportOpenSydeFlashloaderInformationRead(const C_OsyDeviceInformation & orc_Info,
                                                                  const uint32_t ou32_NodeIndex)
{
   // Save the device information
   // Using the signals only for primitive data types when using multi-threading
   this->mpc_Lock->Acquire();
   this->mc_ReportOsyDeviceInformationNodeIndex.push_back(ou32_NodeIndex);
   this->mc_ReportOsyDeviceInformation.push_back(orc_Info);
   this->mpc_Lock->Release();

   Q_EMIT (this->SigReportOpenSydeFlashloaderInformationRead());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports information read from STW Flashloader server node

   Called by ReadDeviceInformation() after it has read information from an STW Flashloader node.
   Here: save device information

   \param[in]     orc_Info          Information read from node
   \param[in]     ou32_NodeIndex   Index of node within mpc_SystemDefinition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSequences::m_ReportStwFlashloaderInformationRead(const C_XflDeviceInformation & orc_Info,
                                                             const uint32_t ou32_NodeIndex)
{
   // Save the device information
   // Using the signals only for primitive data types when using multi-threading
   this->mpc_Lock->Acquire();
   this->mc_ReportXflDeviceInformationNodeIndex.push_back(ou32_NodeIndex);
   this->mc_ReportXflDeviceInformation.push_back(orc_Info);
   this->mpc_Lock->Release();

   Q_EMIT (this->SigReportStwFlashloaderInformationRead());
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpSequences::mh_ThreadFunc(void * const opv_Instance)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_SyvUpSequences * const pc_Sequences = reinterpret_cast<C_SyvUpSequences *>(opv_Instance);

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
void C_SyvUpSequences::m_ThreadFunc(void)
{
   if (this->mpc_ComDriver != NULL)
   {
      switch (this->me_Sequence)
      {
      case eNOT_ACTIVE:
         // Nothing to do. Should not happen.
         break;
      case eACTIVATE_FLASHLOADER:
         this->ms32_Result = this->ActivateFlashloader(false);
         //Ignore error (Same error should be reported with read device information)
         if (this->ms32_Result == C_WARN)
         {
            this->ms32_Result = C_NO_ERR;
         }
         break;
      case eREAD_DEVICE_INFORMATION:
         this->ms32_Result = this->ReadDeviceInformation(false);
         break;
      case eUPDATE_SYSTEM:
         this->ms32_Result = this->UpdateSystem(this->mc_NodesToFlash, this->mc_NodesOrder);
         break;
      case eRESET_SYSTEM:
         this->ms32_Result = this->ResetSystem();
         //Wait until every device is restarted
         stw::tgl::TglSleep(2000);
         break;
      default:
         tgl_assert(false);
         break;
      }
   }
   else
   {
      this->ms32_Result = C_CONFIG;
   }

   this->me_Sequence = eNOT_ACTIVE;

   this->mpc_Thread->requestInterruption();
}

//----------------------------------------------------------------------------------------------------------------------

void C_SyvUpSequences::mh_WriteLog(const C_OscSuSequences::E_ProgressStep oe_Step,
                                   const stw::scl::C_SclString & orc_Text)
{
   // Decide if error or info
   switch (oe_Step) //lint !e788  //not all cases handled explicitly here
   {
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_ECU_RESET_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_ROUTING_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_ROUTING_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_RECONNECT_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_READ_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_RECONNECT_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_MAX_SIZE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_OPEN_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_EXTRACT_KEY_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SEND_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SEND_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SEND_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR:
   case C_OscSuSequences::eRESET_SYSTEM_OSY_NODE_ERROR:
   case C_OscSuSequences::eRESET_SYSTEM_OSY_ROUTED_NODE_ERROR:
      osc_write_log_error("Update Node", orc_Text);
      break;
   default:
      osc_write_log_info("Update Node", orc_Text);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
