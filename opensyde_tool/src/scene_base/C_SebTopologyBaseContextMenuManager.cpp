//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "gitypes.hpp"
#include "C_GtGetText.hpp"
#include "C_GiLiLineGroup.hpp"
#include "C_SebTopologyBaseContextMenuManager.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
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
C_SebTopologyBaseContextMenuManager::C_SebTopologyBaseContextMenuManager(void) :
   C_SebBaseContextMenuManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebTopologyBaseContextMenuManager::~C_SebTopologyBaseContextMenuManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Activate single item specific actions

   \return
   true     Specific action found
   false    No specific action found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SebTopologyBaseContextMenuManager::m_ActivateSpecificActions(void)
{
   const C_GiLiLineGroup * const pc_LineGroup = dynamic_cast<C_GiLiLineGroup *>(this->mpc_ActiveItem);
   bool q_Return = false;

   if (pc_LineGroup != NULL)
   {
      // Special case: Points can be removed if there are more than 2 points
      if (pc_LineGroup->GetNumberPoints() > 2)
      {
         //Check bend point
         if (pc_LineGroup->CheckBendPointAt(this->mc_ScenePos) == true)
         {
            this->mpc_ActionRemoveBendLine->setVisible(true);
            q_Return = true;
         }
         else
         {
            this->mpc_ActionRemoveBendLine->setVisible(false);
         }
      }
      else
      {
         this->mpc_ActionRemoveBendLine->setVisible(false);
      }
   }
   else
   {
      this->mpc_ActionRemoveBendLine->setVisible(false);
   }
   // specific functionality
   switch (this->mpc_ActiveItem->type())
   {
   // check for bus
   case ms32_GRAPHICS_ITEM_BUS:           // buses have the same functionality
   case ms32_GRAPHICS_ITEM_CANBUS:        // buses have the same functionality
   case ms32_GRAPHICS_ITEM_ETHERNETBUS:   // buses have the same functionality
   case ms32_GRAPHICS_ITEM_BUS_CONNECT:   // connector have the same functionality
   case ms32_GRAPHICS_ITEM_LINE_ARROW:    // connector have the same functionality
   case ms32_GRAPHICS_ITEM_PC_CONNECTION: // connector have the same functionality
      this->mpc_ActionBendLine->setVisible(true);
      q_Return = true;
      break;
   default:
      //No additional actions required
      break;
   }

   return q_Return;
}
