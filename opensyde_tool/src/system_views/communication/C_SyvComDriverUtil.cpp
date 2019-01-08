//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility class for C_OSCComDriver (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.12.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFile>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SyvComDriverUtil.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Fills all parameter for C_OSCComDriverProtocol based on specific view and prepares the CAN dispatcher

   \param[in]   ou32_ViewIndex          Index of current used view
   \param[out]  ou32_ActiveBusIndex     Bus index of bus in system definition where we are connected to
   \param[out]  orc_ActiveNodes         Flags for all available nodes in the system
   \param[out]  opc_CanDispatcher       Pointer to concrete CAN dispatcher
   \param[out]  opc_IpDispatcher        Pointer to concrete IP dispatcher

   \return
   C_NO_ERR    Parameter filled
   C_CONFIG    Invalid system definition/view configuration
   C_BUSY      System view error detected
   C_COM       Initialization of CAN Dispatcher failed
   C_RD_WR     Invalid CAN-DLL path

   \created     14.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvComDriverUtil::h_GetOSCComDriverParamFromView(const uint32 ou32_ViewIndex, uint32 & oru32_ActiveBusIndex,
                                                          std::vector<uint8> & orc_ActiveNodes,
                                                          stw_can::C_CAN ** const oppc_CanDispatcher,
                                                          C_OSCIpDispatcherWinSock ** const oppc_IpDispatcher)
{
   bool q_NameInvalid;
   bool q_PCNotConnected;
   bool q_RoutingInvalid;
   bool q_UpdateDisabledButDataBlocks;
   bool q_SysDefInvalid;
   bool q_RailsInvalid;
   bool q_NoNodesActive;

   sint32 s32_Retval = C_PuiSvHandler::h_GetInstance()->CheckViewError(ou32_ViewIndex, &q_NameInvalid,
                                                                       &q_PCNotConnected, &q_RoutingInvalid,
                                                                       &q_UpdateDisabledButDataBlocks,
                                                                       &q_SysDefInvalid, &q_NoNodesActive,
                                                                       &q_RailsInvalid);

   if ((((((((s32_Retval == C_NO_ERR) && (q_NameInvalid == false)) && (q_PCNotConnected == false)) &&
           (q_RoutingInvalid == false)) && (q_SysDefInvalid == false)) && (q_NoNodesActive == false)) &&
        (q_RailsInvalid == false)) && (q_UpdateDisabledButDataBlocks == false))
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
            pc_View->GetPcData().GetBusIndex());
         if (pc_Bus != NULL)
         {
            //No check for connected because error check passed
            oru32_ActiveBusIndex = pc_View->GetPcData().GetBusIndex();
            orc_ActiveNodes.clear();
            orc_ActiveNodes = pc_View->GetNodeActiveFlags();

            if (pc_Bus->e_Type == C_OSCSystemBus::eCAN)
            {
               QFile c_File;
               QString c_FilePath;
               //No ethernet
               *oppc_IpDispatcher = NULL;

               c_FilePath = pc_View->GetPcData().GetCANDllAbsolute();

               c_File.setFileName(c_FilePath);

               if (c_File.exists() == true)
               {
                  *oppc_CanDispatcher = new stw_can::C_CAN();

                  s32_Retval = (*oppc_CanDispatcher)->DLL_Open(c_FilePath.toStdString().c_str());
                  if (s32_Retval == C_NO_ERR)
                  {
                     s32_Retval = (*oppc_CanDispatcher)->CAN_Init(static_cast<sint32>(pc_Bus->u64_BitRate / 1000ULL));
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
               *oppc_CanDispatcher = NULL;

               *oppc_IpDispatcher = new C_OSCIpDispatcherWinSock();
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
