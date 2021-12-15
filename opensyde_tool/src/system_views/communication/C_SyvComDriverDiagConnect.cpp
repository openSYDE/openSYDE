//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Thread for C_SyvComDriverDiag connect steps

   Thread for C_SyvComDriverDiag connect steps

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "TGLTime.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"
#include "C_SyvComDriverDiagConnect.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComDriverDiagConnect::C_SyvComDriverDiagConnect(QObject * const opc_Parent) :
   QThread(opc_Parent),
   mu32_DisconnectTime(0UL),
   me_ConnectState(eCDCS_UNINITIALIZED),
   mpc_ComDriverDiag(NULL),
   ms32_OperationResult(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current state

   \return
   Current state
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvComDriverDiagConnect::E_ConnectState C_SyvComDriverDiagConnect::GetStep(void) const
{
   return this->me_ConnectState;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set parameters for waiting step

   \param[in] ou32_DisconnectTime Last known disconnect time
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiagConnect::SetWaitingStepParameters(const uint32 ou32_DisconnectTime)
{
   this->mu32_DisconnectTime = ou32_DisconnectTime;
   //Update status
   this->me_ConnectState = eCDCS_WAITING;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set parameters for set diagnostic mode step

   \param[in] opc_ComDriver Actrive com driver to work with
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiagConnect::SetSetDiagnosticModeParameters(C_SyvComDriverDiag * const opc_ComDriver)
{
   this->mpc_ComDriverDiag = opc_ComDriver;
   //Update status
   this->me_ConnectState = eCDCS_SET_DIAGNOSTIC_MODE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set parameters for set up cyclic transmissions step

   \param[in] opc_ComDriver Actrive com driver to work with
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiagConnect::SetSetUpCyclicTransmissionsParameters(C_SyvComDriverDiag * const opc_ComDriver)
{
   this->mpc_ComDriverDiag = opc_ComDriver;
   //Update status
   this->me_ConnectState = eCDCS_SET_UP_CYCLIC_TRANSMISSIONS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get results from last operation

   \param[out] ors32_Result       Result value
   \param[out] orc_Message        Error message (if any)
   \param[out] orc_MessageDetails Error message details (if any)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiagConnect::GetLastOperationResult(sint32 & ors32_Result, QString & orc_Message,
                                                       QString & orc_MessageDetails) const
{
   ors32_Result = this->ms32_OperationResult;
   orc_Message = this->mc_ErrorMessage;
   orc_MessageDetails = this->mc_ErrorMessageDetails;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Execute thread
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiagConnect::run(void)
{
   if (this->me_ConnectState == eCDCS_WAITING)
   {
      m_RunWaitingStep();
      //Update status
      this->me_ConnectState = eCDCS_WAITING_FINISHED;
   }
   else if (this->me_ConnectState == eCDCS_SET_DIAGNOSTIC_MODE)
   {
      m_RunSetDiagnosticMode();
      //Update status
      this->me_ConnectState = eCDCS_SET_DIAGNOSTIC_MODE_FINISHED;
   }
   else if (this->me_ConnectState == eCDCS_SET_UP_CYCLIC_TRANSMISSIONS)
   {
      m_RunSetUpCyclicTransmissions();
      //Update status
      this->me_ConnectState = eCDCS_SET_UP_CYCLIC_TRANSMISSIONS_FINISHED;
   }
   else
   {
      //Nothing to do
      tgl_assert(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Run step: waiting for last session timeout
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiagConnect::m_RunWaitingStep(void)
{
   //If someone requests the results of this step...
   this->mc_ErrorMessage = "";
   this->mc_ErrorMessageDetails = "";
   this->ms32_OperationResult = C_NO_ERR;
   // Is a new connect already possible
   if ((this->mu32_DisconnectTime + 5100U) > TGL_GetTickCount())
   {
      QThread::usleep((this->mu32_DisconnectTime + 5100U) - TGL_GetTickCount());
   }
   while ((this->mu32_DisconnectTime + 5100U) > TGL_GetTickCount())
   {
      // Wait till it is possible
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Run step: set diagnostic mode for all participating nodes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiagConnect::m_RunSetDiagnosticMode(void)
{
   this->mc_ErrorMessage = "";
   this->mc_ErrorMessageDetails = "";

   tgl_assert(this->mpc_ComDriverDiag != NULL);
   if (this->mpc_ComDriverDiag != NULL)
   {
      QString c_ErrorDetails;

      this->ms32_OperationResult = this->mpc_ComDriverDiag->SetDiagnosticMode(c_ErrorDetails);

      switch (this->ms32_OperationResult)
      {
      case C_NO_ERR:
         break;
      case C_CONFIG:
         this->mc_ErrorMessage = static_cast<QString>(C_GtGetText::h_GetText("Initialization was not finished."));
         break;
      case C_NOACT:
         this->mc_ErrorMessage =
            static_cast<QString>(C_GtGetText::h_GetText("System View is invalid. Action cannot be performed."));
         break;
      case C_TIMEOUT:
         this->mc_ErrorMessage = static_cast<QString>(C_GtGetText::h_GetText("At least one node is not responding!"));
         this->mc_ErrorMessageDetails =
            static_cast<QString>(C_GtGetText::h_GetText("Affected node(s):\n%1")).arg(c_ErrorDetails);
         break;
      case C_BUSY:
         this->mc_ErrorMessage =
            static_cast<QString>(C_GtGetText::h_GetText("Connecting to at least one node failed!"));
         this->mc_ErrorMessageDetails =
            static_cast<QString>(C_GtGetText::h_GetText("Affected node(s):\n%1\n\n"
                                                        "Possible scenarios:\n"
                                                        "- Wrong configured IP address\n"
                                                        "- Gateway address of node is not configured properly"))
            .arg(c_ErrorDetails);
         break;
      case C_DEFAULT:
         this->mc_ErrorMessage =
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "The Datapool definition differs between client and node.\n"
                                    "Generate the code for all listed nodes in the details again.\n"
                                    "Tip: Also make sure the Datapools are initialized in the application."));
         this->mc_ErrorMessageDetails =
            static_cast<QString>(C_GtGetText::h_GetText("Affected node(s) and Datapool(s):\n%1")).arg(
               c_ErrorDetails);
         break;
      case C_COM:
         this->mc_ErrorMessage = static_cast<QString>(C_GtGetText::h_GetText("Service error response detected."));
         this->mc_ErrorMessageDetails =
            static_cast<QString>(C_GtGetText::h_GetText("Affected node(s):\n%1\n\n"
                                                        "Possible scenario:\n"
                                                        "If these nodes are in Flashloader (Try restarting those nodes to fix this error)"))
            .arg(c_ErrorDetails);
         break;
      case C_RD_WR:
         this->mc_ErrorMessage = static_cast<QString>(C_GtGetText::h_GetText("Unexpected protocol response detected."));
         this->mc_ErrorMessageDetails = static_cast<QString>(C_GtGetText::h_GetText("Possible scenario:\n"
                                                                                    "If there are multiple nodes with the same node ID connected to the same bus"));
         break;
      case C_WARN:
         this->mc_ErrorMessage = static_cast<QString>(C_GtGetText::h_GetText("Protocol error response detected."));
         this->mc_ErrorMessageDetails = static_cast<QString>(C_GtGetText::h_GetText("Possible scenario:\n"
                                                                                    "- Node can currently not enter diagnostic session\n"
                                                                                    "- If the node is in Flashloader (Try restarting the node to fix this error)"));
         break;
      case C_CHECKSUM:
         this->mc_ErrorMessage =
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "Authentication between openSYDE Tool and device(s) has failed. Access denied."));
         C_OSCLoggingHandler::h_Flush();
         this->mc_ErrorMessageDetails = C_GtGetText::h_GetText("Possible reasons:<br/>"
                                                               "- Associated private key (*.pem) not found in /certificates folder (most common)<br/>"
                                                               "- Failure during authenfication process<br/>"
                                                               "For more information see ") +
                                        C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                                                         C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()) +
                                        C_GtGetText::h_GetText(".");
         break;
      default:
         this->mc_ErrorMessage =
            static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(this->
                                                                                                    ms32_OperationResult));
         break;
      }
   }
   else
   {
      this->ms32_OperationResult = C_CONFIG;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Run step: set up all necessary cyclic transmissions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvComDriverDiagConnect::m_RunSetUpCyclicTransmissions(void)
{
   this->mc_ErrorMessage = "";
   this->mc_ErrorMessageDetails = "";
   tgl_assert(this->mpc_ComDriverDiag != NULL);
   if (this->mpc_ComDriverDiag != NULL)
   {
      QString c_ErrorDetails;
      std::vector<C_OSCNodeDataPoolListElementId> c_FailedIdRegisters;
      std::vector<QString> c_FailedIdErrorDetails;
      std::map<stw_types::uint32, stw_types::uint32> c_FailedNodesElementNumber;
      std::map<stw_types::uint32, stw_types::uint32> c_NodesElementNumber;

      // Start the asyn communication
      this->ms32_OperationResult = this->mpc_ComDriverDiag->SetUpCyclicTransmissions(c_ErrorDetails,
                                                                                     c_FailedIdRegisters,
                                                                                     c_FailedIdErrorDetails,
                                                                                     c_FailedNodesElementNumber,
                                                                                     c_NodesElementNumber);

      switch (this->ms32_OperationResult)
      {
      case C_NO_ERR:
         if (c_FailedIdRegisters.size() > 0)
         {
            uintn un_Counter;
            uintn un_CurrentNode = 0xFFFFFFFFU;

            // Show all error information
            this->mc_ErrorMessage =
               static_cast<QString>(C_GtGetText::h_GetText(
                                       "Not all cyclic transmission could be registered. (%1 failed)")).
               arg(
                  c_FailedIdRegisters.size());

            for (un_Counter = 0; un_Counter < c_FailedIdRegisters.size(); ++un_Counter)
            {
               const C_OSCNodeDataPoolListElementId & rc_Id = c_FailedIdRegisters[un_Counter];
               C_OSCNodeDataPool::E_Type e_Type;
               C_PuiSdHandler::h_GetInstance()->GetDataPoolType(rc_Id.u32_NodeIndex, rc_Id.u32_DataPoolIndex,
                                                                e_Type);

               if (un_CurrentNode != rc_Id.u32_NodeIndex)
               {
                  const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                     rc_Id.u32_NodeIndex);

                  if (this->mc_ErrorMessageDetails != "")
                  {
                     this->mc_ErrorMessageDetails += "\n\n";
                  }

                  if (pc_Node != NULL)
                  {
                     const std::map<stw_types::uint32,
                                    stw_types::uint32>::const_iterator c_ItFailedNodesElementNumber =
                        c_FailedNodesElementNumber.find(rc_Id.u32_NodeIndex);

                     // This information as title for all errors of this node
                     this->mc_ErrorMessageDetails += "Node " +
                                                     static_cast<QString>(pc_Node->c_Properties.c_Name.c_str()) + ":\n";

                     if (c_ItFailedNodesElementNumber != c_FailedNodesElementNumber.end())
                     {
                        this->mc_ErrorMessageDetails +=
                           static_cast<QString>("The node %1 can handle maximum %2 transmissions. ").
                           arg(pc_Node->c_Properties.c_Name.c_str()).
                           arg(c_FailedNodesElementNumber[rc_Id.u32_NodeIndex] - 1U);
                        this->mc_ErrorMessageDetails += static_cast<QString>("Tried to register %1 transmissions.\n"
                                                                             "To fix this adjust 'Max number of cyclic/event driven "
                                                                             "transmissions' in 'File Generation Settings' of"
                                                                             " the node's properties.\n\n").
                                                        arg(c_NodesElementNumber[rc_Id.u32_NodeIndex]);
                     }
                  }
                  this->mc_ErrorMessageDetails += "Error details:\n";

                  un_CurrentNode = rc_Id.u32_NodeIndex;
               }

               this->mc_ErrorMessageDetails += "The initiating of the transmission of the element ";
               if (e_Type != C_OSCNodeDataPool::eCOM)
               {
                  this->mc_ErrorMessageDetails += C_PuiSdUtil::h_GetNamespace(rc_Id);
               }
               else
               {
                  this->mc_ErrorMessageDetails += C_PuiSdUtil::h_GetSignalNamespace(rc_Id);
               }

               this->mc_ErrorMessageDetails += " failed: " + c_FailedIdErrorDetails[un_Counter] + "\n";
            }

            this->ms32_OperationResult = C_RD_WR;
         }

         break;
      case C_CONFIG:
         this->mc_ErrorMessage =
            static_cast<QString>(C_GtGetText::h_GetText("System connection failed. Configuration error."));
         //Many  possibilities for this error
         break;
      case C_COM:
         this->mc_ErrorMessage =
            static_cast<QString>(C_GtGetText::h_GetText("Communication error. (%1)")).arg(c_ErrorDetails);
         break;
      default:
         this->mc_ErrorMessage =
            static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).
            arg(C_Uti::h_StwError(this->ms32_OperationResult));
         break;
      }
   }
   else
   {
      this->ms32_OperationResult = C_CONFIG;
   }
}
