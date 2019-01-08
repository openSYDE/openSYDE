//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       short description (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "gitypes.h"
#include "C_GtGetText.h"
#include "C_GiLiLineGroup.h"
#include "C_SebTopologyBaseContextMenuManager.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     03.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SebTopologyBaseContextMenuManager::C_SebTopologyBaseContextMenuManager(void) :
   C_SebBaseContextMenuManager()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     03.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SebTopologyBaseContextMenuManager::~C_SebTopologyBaseContextMenuManager(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activate single item specific actions

   \return
   true     Specific action found
   false    No specific action found

   \created     03.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SebTopologyBaseContextMenuManager::m_ActivateSpecificActions(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_GiLiLineGroup * pc_LineGroup = dynamic_cast<C_GiLiLineGroup *>(this->mpc_ActiveItem);
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
   case msn_GRAPHICS_ITEM_BUS:           // buses have the same functionality
   case msn_GRAPHICS_ITEM_CANBUS:        // buses have the same functionality
   case msn_GRAPHICS_ITEM_ETHERNETBUS:   // buses have the same functionality
   case msn_GRAPHICS_ITEM_BUS_CONNECT:   // connector have the same functionality
   case msn_GRAPHICS_ITEM_LINE_ARROW:    // connector have the same functionality
   case msn_GRAPHICS_ITEM_PC_CONNECTION: // connector have the same functionality
      this->mpc_ActionBendLine->setVisible(true);
      q_Return = true;
      break;
   default:
      //No additional actions required
      break;
   }

   return q_Return;
}
