//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Informational Line / Arrow for user in system view dashboard (implementation)

   Informational Line / Arrow for user in system view dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiSvDaArrow.hpp"
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
   \param[in]     opc_Points           Points for line
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaArrow::C_GiSvDaArrow(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                             const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                             const std::vector<QPointF> * const opc_Points, QGraphicsItem * const opc_Parent) :
   C_GiBiArrow(oru64_Id, opc_Points, opc_Parent),
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
                  pc_Dashboard->GetLineArrow(static_cast<uint32_t>(this->ms32_Index));
               if (pc_Item != NULL)
               {
                  m_Init(pc_Item->c_UiInteractionPoints);
                  this->C_GiSvDaArrow::LoadData();
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
            const C_PuiBsLineArrow * const pc_Item =
               pc_Dashboard->GetLineArrow(static_cast<uint32_t>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               C_PuiBsLineArrow c_Copy = *pc_Item;
               //Dark mode work around
               if (pc_View->GetDarkModeActive() == true)
               {
                  c_Copy.c_UiColor = c_Copy.c_UiColorDark;
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
            const C_PuiBsLineArrow * const pc_Item =
               pc_Dashboard->GetLineArrow(static_cast<uint32_t>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               C_PuiBsLineArrow c_Item = *pc_Item;

               m_UpdateData(c_Item);

               //Dark mode work around
               if (pc_View->GetDarkModeActive() == true)
               {
                  c_Item.c_UiColorDark = c_Item.c_UiColor;
                  c_Item.c_UiColor = pc_Item->c_UiColor;
               }

               C_PuiSvHandler::h_GetInstance()->SetDashboardLineArrow(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                      static_cast<uint32_t>(this->ms32_Index), c_Item);
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
                                                                static_cast<uint32_t>(this->ms32_Index));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaArrow::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiBiArrow::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}
