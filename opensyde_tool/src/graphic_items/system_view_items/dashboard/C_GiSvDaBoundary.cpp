//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Implementation for drawing element boundary in system view dashboard (implementation)

   Implementation for drawing element boundary in system view dashboard

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
#include "C_GiSvDaBoundary.h"
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
   \param[in]     orf64_Width          Width of node
   \param[in]     orf64_Height         Height of node
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     20.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaBoundary::C_GiSvDaBoundary(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                   const sint32 & ors32_DataIndex, const uint64 & oru64_ID, const float64 of64_Width,
                                   const float64 of64_Height, QGraphicsItem * const opc_Parent) :
   C_GiBiBoundary(oru64_ID, of64_Width, of64_Height, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eBOUNDARY)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function for initially loading internal data

   \created     10.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
            const C_PuiBsBoundary * const pc_Item = pc_Dashboard->GetBoundary(static_cast<uint32>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               m_LoadFromData(*pc_Item, pc_View->GetDarkModeActive());
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
            const C_PuiBsBoundary * const pc_Item = pc_Dashboard->GetBoundary(static_cast<uint32>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               C_PuiBsBoundary c_Item = *pc_Item;
               m_UpdateData(c_Item, pc_View->GetDarkModeActive());
               C_PuiSvHandler::h_GetInstance()->SetDashboardBoundary(this->mu32_ViewIndex, this->mu32_DashboardIndex,
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
void C_GiSvDaBoundary::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiSvHandler::h_GetInstance()->DeleteDashboardBoundary(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                               static_cast<uint32>(this->ms32_Index));
   }
}
