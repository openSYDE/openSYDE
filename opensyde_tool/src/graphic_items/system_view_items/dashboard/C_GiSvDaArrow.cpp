//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Informational Line / Arrow for user in system view dashboard (implementation)

   Informational Line / Arrow for user in system view dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GiSvDaArrow.h"
#include "C_PuiSvHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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
   \param[in]     oru64_ID             Unique ID
   \param[in]     opc_Points           Points for line
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaArrow::C_GiSvDaArrow(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                             const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                             const std::vector<QPointF> * const opc_Points, QGraphicsItem * const opc_Parent) :
   C_GiBiArrow(oru64_ID, opc_Points, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eLINE_ARROW)
{
   if (opc_Points == NULL)
   {
      if (this->ms32_Index >= 0)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         if (pc_View != NULL)
         {
            const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
            if (pc_Dashboard != NULL)
            {
               const C_PuiBsLineArrow * const pc_Item =
                  pc_Dashboard->GetLineArrow(static_cast<uint32>(this->ms32_Index));
               if (pc_Item != NULL)
               {
                  Init(pc_Item->c_UIInteractionPoints);
                  this->LoadData();
               }
            }
         }
      }
   }

   m_GenerateArrows();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaArrow::LoadData(void)
{
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         if (pc_Dashboard != NULL)
         {
            const C_PuiBsLineArrow * const pc_Item = pc_Dashboard->GetLineArrow(static_cast<uint32>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               C_PuiBsLineArrow c_Copy = *pc_Item;
               //Dark mode work around
               if (pc_View->GetDarkModeActive() == true)
               {
                  c_Copy.c_UIColor = c_Copy.c_UIColorDark;
               }
               m_LoadFromData(c_Copy);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaArrow::UpdateData(void)
{
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         if (pc_Dashboard != NULL)
         {
            const C_PuiBsLineArrow * const pc_Item = pc_Dashboard->GetLineArrow(static_cast<uint32>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               C_PuiBsLineArrow c_Item = *pc_Item;

               m_UpdateData(c_Item);

               //Dark mode work around
               if (pc_View->GetDarkModeActive() == true)
               {
                  c_Item.c_UIColorDark = c_Item.c_UIColor;
                  c_Item.c_UIColor = pc_Item->c_UIColor;
               }

               C_PuiSvHandler::h_GetInstance()->SetDashboardLineArrow(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                      static_cast<uint32>(this->ms32_Index), c_Item);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaArrow::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiSvHandler::h_GetInstance()->DeleteDashboardLineArrow(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                static_cast<uint32>(this->ms32_Index));
   }
}
