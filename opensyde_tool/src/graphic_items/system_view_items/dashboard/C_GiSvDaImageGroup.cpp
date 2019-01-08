//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Implementation for drawing element image in system view dashboard (implementation)

   Implementation for drawing element image in system view dashboard

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
#include "C_GiSvDaImageGroup.h"
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
   \param[in]     orc_ImagePath        File path of image
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     20.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaImageGroup::C_GiSvDaImageGroup(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                       const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                       const QString & orc_ImagePath, QGraphicsItem * const opc_Parent) :
   C_GiBiImageGroup(oru64_ID, orc_ImagePath, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eIMAGE)
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
   \param[in]     orc_Image            Image
   \param[in]     orf64_Width          Width of image view
   \param[in]     orf64_Height          Height of image view
   \param[in]     orc_ImagePath        Filepath to the image
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     20.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaImageGroup::C_GiSvDaImageGroup(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                       const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                       const float64 of64_Width, const float64 of64_Height, const QPixmap & orc_Image,
                                       QGraphicsItem * const opc_Parent) :
   C_GiBiImageGroup(oru64_ID, of64_Width, of64_Height, orc_Image, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eIMAGE)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function for initially loading internal data

   \created     10.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaImageGroup::LoadData(void)
{
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
         if (pc_Dashboard != NULL)
         {
            const C_PuiBsImage * const pc_Item = pc_Dashboard->GetImage(static_cast<uint32>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               this->LoadBasicData(*pc_Item);
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
void C_GiSvDaImageGroup::UpdateData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiBsImage c_Item;
      m_UpdateData(c_Item);
      C_PuiSvHandler::h_GetInstance()->SetDashboardImage(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                         static_cast<uint32>(this->ms32_Index), c_Item);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete data in system definition

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaImageGroup::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiSvHandler::h_GetInstance()->DeleteDashboardImage(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                            static_cast<uint32>(this->ms32_Index));
   }
}
