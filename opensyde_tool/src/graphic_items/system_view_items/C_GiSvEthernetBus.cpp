//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       System view ethernet bus (implementation)

   System view ethernet bus

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_PuiSvHandler.h"
#include "C_GiSvEthernetBus.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
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

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex       Index of system view
   \param[in]     ors32_Index          Index of data element in system definition
   \param[in]     oru64_ID             Unique ID
   \param[in]     opc_TextElementName  Pointer to text element for showing bus name
   \param[in]     opc_Points           Points for line
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     15.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvEthernetBus::C_GiSvEthernetBus(const uint32 ou32_ViewIndex, const sint32 & ors32_Index, const uint64 & oru64_ID,
                                     C_GiTextElementBus * const opc_TextElementName,
                                     const std::vector<QPointF> * const opc_Points, QGraphicsItem * const opc_Parent) :
   C_GiLiEthernetBus(ors32_Index, oru64_ID, opc_TextElementName, false, opc_Points, opc_Parent),
   mu32_ViewIndex(ou32_ViewIndex)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checking the bus data for errors and updates the conflict icon

   \created     15.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvEthernetBus::CheckBusForChanges(void)
{
   if (this->mpc_TextElementName != NULL)
   {
      if (C_PuiSvHandler::h_GetInstance()->CheckBusDisabled(this->mu32_ViewIndex,
                                                            static_cast<uint32>(this->ms32_Index)) == false)
      {
         this->mpc_TextElementName->SetErrorState(C_PuiSvHandler::h_GetInstance()->GetErrorBus(this->ms32_Index));
      }
      else
      {
         this->mpc_TextElementName->SetErrorState(false);
      }

      //check for new name
      this->mpc_TextElementName->SetText(this->GetName());
      this->mpc_TextElementName->update();
   }
}
