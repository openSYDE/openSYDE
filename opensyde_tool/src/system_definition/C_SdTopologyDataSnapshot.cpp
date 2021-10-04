//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Snapshot of system definition items (implementation)

   Snapshot of system definition items

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "C_SebUtil.h"
#include "C_PuiSdUtil.h"
#include "C_SdTopologyDataSnapshot.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

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
C_SdTopologyDataSnapshot::C_SdTopologyDataSnapshot(void) :
   C_PuiBsElements()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdTopologyDataSnapshot::~C_SdTopologyDataSnapshot(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all data storage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyDataSnapshot::Clear(void)
{
   this->c_Boundaries.clear();
   this->c_Images.clear();
   this->c_LineArrows.clear();
   this->c_TextElements.clear();
   this->c_UINodes.clear();
   this->c_UIBuses.clear();
   this->c_OSCNodes.clear();
   this->c_OSCNodeGroups.clear();
   this->c_OSCBuses.clear();
   this->c_BusConnections.clear();
   this->c_BusTextElements.clear();
   C_PuiBsElements::Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Count sum of all items

   \return
   sum of all items
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdTopologyDataSnapshot::Count(void) const
{
   uint32 u32_Retval = 0;

   u32_Retval += C_PuiBsElements::Count();
   for (uint32 u32_ItNode = 0; u32_ItNode < this->c_UINodes.size(); ++u32_ItNode)
   {
      const bool q_IsFirst = C_PuiSdUtil::h_CheckIsFirstInAnyGroupOrNotInAny(u32_ItNode, this->c_OSCNodeGroups);
      if (q_IsFirst)
      {
         const C_PuiSdNode & rc_UINode = this->c_UINodes[u32_ItNode];
         //Node
         ++u32_Retval;
         //Connections
         u32_Retval += rc_UINode.c_UIBusConnections.size();
      }
   }

   u32_Retval += this->c_UIBuses.size();
   u32_Retval += this->c_BusConnections.size();
   u32_Retval += this->c_BusTextElements.size();

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply position offset

   \param[in]  orc_NewPos  Offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyDataSnapshot::SetDataPositionOffset(const QPointF & orc_NewPos)
{
   C_PuiBsElements::SetDataPositionOffset(orc_NewPos);
   //Buses
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->c_UIBuses.size(); ++u32_ItItem)
   {
      C_PuiSdBus & rc_Data = this->c_UIBuses[u32_ItItem];
      C_SebUtil::h_AddLineOffset(rc_Data, orc_NewPos);
   }
   //Bus text elements
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->c_BusTextElements.size(); ++u32_ItItem)
   {
      C_PuiSdTextElementBus & rc_Data = this->c_BusTextElements[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Nodes
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->c_UINodes.size(); ++u32_ItItem)
   {
      C_PuiSdNode & rc_Data = this->c_UINodes[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Connections
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->c_BusConnections.size(); ++u32_ItItem)
   {
      C_PuiSdCompleteBusConnectionData & rc_Data = this->c_BusConnections[u32_ItItem];
      for (uint32 u32_ItConnectorPoint = 0;
           u32_ItConnectorPoint < rc_Data.c_UIData.c_UINodeConnectionInteractionPoints.size();
           ++u32_ItConnectorPoint)
      {
         rc_Data.c_UIData.c_UINodeConnectionInteractionPoints[u32_ItConnectorPoint] += orc_NewPos;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply Z value offset

   \param[in]  of64_HighestUsedZValue  Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyDataSnapshot::SetDataZOffset(const float64 of64_HighestUsedZValue)
{
   C_PuiBsElements::SetDataZOffset(of64_HighestUsedZValue);
   //Buses
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->c_UIBuses.size(); ++u32_ItItem)
   {
      C_PuiSdBus & rc_Data = this->c_UIBuses[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Bus text elements
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->c_BusTextElements.size(); ++u32_ItItem)
   {
      C_PuiSdTextElementBus & rc_Data = this->c_BusTextElements[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Nodes
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->c_UINodes.size(); ++u32_ItItem)
   {
      C_PuiSdNode & rc_Data = this->c_UINodes[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Connections
   //No Z order
}
