//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Snapshot of system definition items (implementation)

   Snapshot of system definition items

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "C_SebUtil.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_SdTopologyDataSnapshot.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
   this->c_UiNodes.clear();
   this->c_UiBuses.clear();
   this->c_OscNodes.clear();
   this->c_OscNodeGroups.clear();
   this->c_OscBuses.clear();
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
uint32_t C_SdTopologyDataSnapshot::Count(void) const
{
   uint32_t u32_Retval = 0;

   u32_Retval += C_PuiBsElements::Count();
   for (uint32_t u32_ItNode = 0; u32_ItNode < this->c_UiNodes.size(); ++u32_ItNode)
   {
      const bool q_IsFirst = C_PuiSdUtil::h_CheckIsFirstInAnyGroupOrNotInAny(u32_ItNode, this->c_OscNodeGroups);
      if (q_IsFirst)
      {
         const C_PuiSdNode & rc_UiNode = this->c_UiNodes[u32_ItNode];
         //Node
         ++u32_Retval;
         //Connections
         u32_Retval += rc_UiNode.c_UiBusConnections.size();
      }
   }

   u32_Retval += this->c_UiBuses.size();
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
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < this->c_UiBuses.size(); ++u32_ItItem)
   {
      C_PuiSdBus & rc_Data = this->c_UiBuses[u32_ItItem];
      C_SebUtil::h_AddLineOffset(rc_Data, orc_NewPos);
   }
   //Bus text elements
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < this->c_BusTextElements.size(); ++u32_ItItem)
   {
      C_PuiSdTextElementBus & rc_Data = this->c_BusTextElements[u32_ItItem];
      rc_Data.c_UiPosition += orc_NewPos;
   }
   //Nodes
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < this->c_UiNodes.size(); ++u32_ItItem)
   {
      C_PuiSdNode & rc_Data = this->c_UiNodes[u32_ItItem];
      rc_Data.c_UiPosition += orc_NewPos;
   }
   //Connections
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < this->c_BusConnections.size(); ++u32_ItItem)
   {
      C_PuiSdCompleteBusConnectionData & rc_Data = this->c_BusConnections[u32_ItItem];
      for (uint32_t u32_ItConnectorPoint = 0;
           u32_ItConnectorPoint < rc_Data.c_UiData.c_UiNodeConnectionInteractionPoints.size();
           ++u32_ItConnectorPoint)
      {
         rc_Data.c_UiData.c_UiNodeConnectionInteractionPoints[u32_ItConnectorPoint] += orc_NewPos;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply Z value offset

   \param[in]  of64_HighestUsedZetValue  Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyDataSnapshot::SetDataZetOffset(const float64_t of64_HighestUsedZetValue)
{
   C_PuiBsElements::SetDataZetOffset(of64_HighestUsedZetValue);
   //Buses
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < this->c_UiBuses.size(); ++u32_ItItem)
   {
      C_PuiSdBus & rc_Data = this->c_UiBuses[u32_ItItem];
      rc_Data.f64_ZetOrder += of64_HighestUsedZetValue;
   }
   //Bus text elements
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < this->c_BusTextElements.size(); ++u32_ItItem)
   {
      C_PuiSdTextElementBus & rc_Data = this->c_BusTextElements[u32_ItItem];
      rc_Data.f64_ZetOrder += of64_HighestUsedZetValue;
   }
   //Nodes
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < this->c_UiNodes.size(); ++u32_ItItem)
   {
      C_PuiSdNode & rc_Data = this->c_UiNodes[u32_ItItem];
      rc_Data.f64_ZetOrder += of64_HighestUsedZetValue;
   }
   //Connections
   //No Z order
}
