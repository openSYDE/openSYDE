//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom text element graphics item in system view dashboard (implementation)

   Custom text element graphics item in system view dashboard

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GiSvDaTextElement.h"
#include "C_PuiSvHandler.h"

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

   \param[in]     oru32_ViewIndex      Index of system view
   \param[in]     oru32_DashboardIndex Index of dashboard in system view
   \param[in]     ors32_DataIndex      Index of connected data item
   \param[in]     oru64_ID             Unique ID
   \param[in]     oq_Editable          Flag for editing the content of the text element
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     20.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaTextElement::C_GiSvDaTextElement(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                         const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                         const bool oq_Editable, QGraphicsItem * const opc_Parent) :
   C_GiBiTextElement(oru64_ID, oq_Editable, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eTEXT_ELEMENT)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     oru32_ViewIndex      Index of system view
   \param[in]     oru32_DashboardIndex Index of dashboard in system view
   \param[in]     ors32_DataIndex      Index of connected data item
   \param[in]     oru64_ID             Unique ID
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     20.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaTextElement::C_GiSvDaTextElement(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                         const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                         QGraphicsItem * const opc_Parent) :
   C_GiBiTextElement(oru64_ID, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eTEXT_ELEMENT)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function for initially loading internal data

   \created     10.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaTextElement::LoadData(void)
{
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         if (pc_Dashboard != NULL)
         {
            const C_PuiBsTextElement * const pc_Item = pc_Dashboard->GetTextElement(
               static_cast<uint32>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               this->m_LoadTextElementData(pc_Item, pc_View->GetDarkModeActive());
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for updating internal data

   \created     02.11.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvDaTextElement::UpdateData(void)
{
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         if (pc_Dashboard != NULL)
         {
            const C_PuiBsTextElement * const pc_Item = pc_Dashboard->GetTextElement(
               static_cast<uint32>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               C_PuiBsTextElement c_Item = *pc_Item;

               this->m_UpdateTextElementData(&c_Item, pc_View->GetDarkModeActive());

               C_PuiSvHandler::h_GetInstance()->SetDashboardTextElement(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index), c_Item);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete data in system definition

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaTextElement::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiSvHandler::h_GetInstance()->DeleteDashboardTextElement(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                  static_cast<uint32>(this->ms32_Index));
   }
}
