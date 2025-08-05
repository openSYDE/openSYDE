//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image in system view dashboard (implementation)

   Implementation for drawing element image in system view dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiSvDaImageGroup.hpp"
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

   \param[in]      oru32_ViewIndex        Index of system view
   \param[in]      oru32_DashboardIndex   Index of dashboard in system view
   \param[in]      ors32_DataIndex        Index of connected data item
   \param[in]      oru64_Id               Unique ID
   \param[in]      orc_ImagePath          File path of image
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaImageGroup::C_GiSvDaImageGroup(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                       const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                       const QString & orc_ImagePath, QGraphicsItem * const opc_Parent) :
   C_GiBiImageGroup(oru64_Id, orc_ImagePath, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eIMAGE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      oru32_ViewIndex        Index of system view
   \param[in]      oru32_DashboardIndex   Index of dashboard in system view
   \param[in]      ors32_DataIndex        Index of connected data item
   \param[in]      oru64_Id               Unique ID
   \param[in]      of64_Width             Width of image view
   \param[in]      of64_Height            Height of image view
   \param[in]      orc_Image              Image
   \param[in]      orc_Format             Image format
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaImageGroup::C_GiSvDaImageGroup(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                       const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                       const float64_t of64_Width, const float64_t of64_Height,
                                       const QPixmap & orc_Image, const QByteArray & orc_Format,
                                       QGraphicsItem * const opc_Parent) :
   C_GiBiImageGroup(oru64_Id, of64_Width, of64_Height, orc_Image, orc_Format, opc_Parent),
   C_PuiSvDbDataElement(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eIMAGE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
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
            const C_PuiBsImage * const pc_Item = pc_Dashboard->GetImage(static_cast<uint32_t>(this->ms32_Index));
            if (pc_Item != NULL)
            {
               this->LoadBasicData(*pc_Item);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaImageGroup::UpdateData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiBsImage c_Item;
      m_UpdateData(c_Item);
      C_PuiSvHandler::h_GetInstance()->SetDashboardImage(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                         static_cast<uint32_t>(this->ms32_Index), c_Item);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaImageGroup::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      C_PuiSvHandler::h_GetInstance()->DeleteDashboardImage(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                            static_cast<uint32_t>(this->ms32_Index));
   }
}
