//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text element graphics item in system view dashboard (implementation)

   Custom text element graphics item in system view dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QTextCursor>

#include "stwtypes.hpp"
#include "C_GiSvDaTextElement.hpp"
#include "C_PuiSvHandler.hpp"

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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     oru32_ViewIndex      Index of system view
   \param[in]     oru32_DashboardIndex Index of dashboard in system view
   \param[in]     ors32_DataIndex      Index of connected data item
   \param[in]     oru64_Id             Unique ID
   \param[in]     oq_Editable          Flag for editing the content of the text element
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaTextElement::C_GiSvDaTextElement(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                         const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                         const bool oq_Editable, QGraphicsItem * const opc_Parent) :
   C_GiBiTextElement(oru64_Id, oq_Editable, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eTEXT_ELEMENT)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     oru32_ViewIndex      Index of system view
   \param[in]     oru32_DashboardIndex Index of dashboard in system view
   \param[in]     ors32_DataIndex      Index of connected data item
   \param[in]     oru64_Id             Unique ID
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaTextElement::C_GiSvDaTextElement(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                         const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                         QGraphicsItem * const opc_Parent) :
   C_GiBiTextElement(oru64_Id, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eTEXT_ELEMENT)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
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
               static_cast<uint32_t>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               this->m_LoadTextElementData(pc_Item, pc_View->GetDarkModeActive());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
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
               static_cast<uint32_t>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               C_PuiBsTextElement c_Item = *pc_Item;

               this->m_UpdateTextElementData(&c_Item, pc_View->GetDarkModeActive());

               C_PuiSvHandler::h_GetInstance()->SetDashboardTextElement(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                        static_cast<uint32_t>(this->ms32_Index),
                                                                        c_Item);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTextElement::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiSvHandler::h_GetInstance()->DeleteDashboardTextElement(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                  static_cast<uint32_t>(this->ms32_Index));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTextElement::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiBiTextElement::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enable edit content mode by enabling the text interaction
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaTextElement::EnableEditContent(void)
{
   QTextCursor c_Cursor = this->mpc_TextItem->textCursor();

   this->mpc_TextItem->SetTextInteraction(true);

   c_Cursor.movePosition(QTextCursor::Start);
   this->mpc_TextItem->setTextCursor(c_Cursor);
}
