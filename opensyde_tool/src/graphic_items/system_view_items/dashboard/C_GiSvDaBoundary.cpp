//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element boundary in system view dashboard (implementation)

   Implementation for drawing element boundary in system view dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiSvDaBoundary.hpp"
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
   \param[in]     of64_Width           Width of node
   \param[in]     of64_Height          Height of node
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaBoundary::C_GiSvDaBoundary(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                   const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                   const float64_t of64_Width, const float64_t of64_Height,
                                   QGraphicsItem * const opc_Parent) :
   C_GiBiBoundary(oru64_Id, of64_Width, of64_Height, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eBOUNDARY)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaBoundary::LoadData(void)
{
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         if (pc_Dashboard != NULL)
         {
            const C_PuiBsBoundary * const pc_Item = pc_Dashboard->GetBoundary(static_cast<uint32_t>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               m_LoadFromData(*pc_Item, pc_View->GetDarkModeActive());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaBoundary::UpdateData(void)
{
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         if (pc_Dashboard != NULL)
         {
            const C_PuiBsBoundary * const pc_Item = pc_Dashboard->GetBoundary(static_cast<uint32_t>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               C_PuiBsBoundary c_Item = *pc_Item;
               m_UpdateData(c_Item, pc_View->GetDarkModeActive());
               C_PuiSvHandler::h_GetInstance()->SetDashboardBoundary(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                     static_cast<uint32_t>(this->ms32_Index), c_Item);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaBoundary::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiSvHandler::h_GetInstance()->DeleteDashboardBoundary(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                               static_cast<uint32_t>(this->ms32_Index));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaBoundary::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiBiBoundary::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}
