//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for C_OscComDriver (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFile>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_Uti.hpp"
#include "C_SyvComDriverUtil.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fills all parameter for C_OscComDriverProtocol based on specific view and prepares the CAN dispatcher

   \param[in]      ou32_ViewIndex                              Index of current used view
   \param[out]     oru32_ActiveBusIndex                        Bus index of bus in system definition where we are
                                                               connected to
   \param[out]     orc_ActiveNodes                             Flags for all available nodes in the system
   \param[in,out]  oppc_CanDispatcher                          Pointer to concrete CAN dispatcher
   \param[in,out]  oppc_IpDispatcher                           Pointer to concrete IP dispatcher
   \param[in]      oq_InitCan                                  Optional flag to initialize the CAN bus. Default is true.
                                                               DLL will be opened although.
   \param[in]      oq_IgnoreUpdateRoutingErrors                Ignoring an update routing invalid error of view check
   \param[out]     opq_DashboardRoutingErrors                  Optional: Flag for a detected dashboard routing error
                                                               of at least one node

   \return
   C_NO_ERR    Parameter filled
   C_CONFIG    Invalid system definition/view configuration
   C_BUSY      System view error detected
   C_COM       Initialization of CAN Dispatcher failed
   C_RD_WR     Invalid CAN-DLL path
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvComDriverUtil::h_GetOscComDriverParamFromView(const uint32_t ou32_ViewIndex,
                                                           uint32_t & oru32_ActiveBusIndex,
                                                           std::vector<uint8_t> & orc_ActiveNodes,
                                                           stw::can::C_Can ** const oppc_CanDispatcher,
                                                           C_OscIpDispatcherWinSock ** const oppc_IpDispatcher,
                                                           const bool oq_InitCan,
                                                           const bool oq_IgnoreUpdateRoutingErrors,
                                                           bool * const opq_DashboardRoutingErrors)
{
   bool q_NameInvalid;
   bool q_PcNotConnected;
   bool q_RoutingInvalid;
   bool q_UpdateRoutingInvalid;
   bool q_SysDefInvalid;
   bool q_NoNodesActive;

   int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->CheckViewError(ou32_ViewIndex, &q_NameInvalid,
                                                                        &q_PcNotConnected, &q_RoutingInvalid,
                                                                        &q_UpdateRoutingInvalid,
                                                                        opq_DashboardRoutingErrors,
                                                                        &q_SysDefInvalid, &q_NoNodesActive,
                                                                        NULL, NULL);

   if ((s32_Retval == C_NO_ERR) && (q_NameInvalid == false) && (q_PcNotConnected == false) &&
       (q_RoutingInvalid == false) && (q_SysDefInvalid == false) && (q_NoNodesActive == false) &&
       ((q_UpdateRoutingInvalid == false) || (oq_IgnoreUpdateRoutingErrors == true)))
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
            pc_View->GetPcData().GetBusIndex());
         if (pc_Bus != NULL)
         {
            //No check for connected because error check passed
            oru32_ActiveBusIndex = pc_View->GetPcData().GetBusIndex();
            orc_ActiveNodes.clear();

            if (C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
                   ou32_ViewIndex,
                   orc_ActiveNodes) == C_NO_ERR)
            {
               if (pc_Bus->e_Type == C_OscSystemBus::eCAN)
               {
                  QFile c_File;
                  QString c_FilePath;
                  //No ethernet
                  *oppc_IpDispatcher = NULL;

                  c_FilePath = pc_View->GetPcData().GetCanDllAbsolute();

                  c_File.setFileName(c_FilePath);

                  if (c_File.exists() == true)
                  {
                     *oppc_CanDispatcher = new stw::can::C_Can();

                     s32_Retval = (*oppc_CanDispatcher)->DLL_Open(c_FilePath.toStdString().c_str());
                     if ((s32_Retval == C_NO_ERR) &&
                         (oq_InitCan == true))
                     {
                        s32_Retval =
                           (*oppc_CanDispatcher)->CAN_Init(static_cast<int32_t>(pc_Bus->u64_BitRate / 1000ULL));
                     }

                     if (s32_Retval != C_NO_ERR)
                     {
                        s32_Retval = C_COM;
                     }
                  }
                  else
                  {
                     s32_Retval = C_RD_WR;
                  }
               }
               else
               {
                  //No CAN

                  // Set optional Ethernet configuration file path
                  const QString c_EthFilePath = stw::opensyde_gui_logic::C_Uti::h_GetAbsolutePathFromExe(
                     "User/eth_config.ini");

                  *oppc_CanDispatcher = NULL;
                  *oppc_IpDispatcher = new C_OscIpDispatcherWinSock();

                  (*oppc_IpDispatcher)->LoadConfigFile(c_EthFilePath.toStdString().c_str());
               }
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_BUSY;
   }

   return s32_Retval;
}
