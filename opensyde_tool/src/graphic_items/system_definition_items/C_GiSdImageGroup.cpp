//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image in system definition (implementation)

   Implementation for drawing element image in system definition

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiSdImageGroup.hpp"
#include "C_PuiSdHandler.hpp"

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
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index   Index of data element in system definition
   \param[in]     oru64_Id      Unique ID
   \param[in]     orc_ImagePath File path of image
   \param[in,out] opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSdImageGroup::C_GiSdImageGroup(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                   const QString & orc_ImagePath, QGraphicsItem * const opc_Parent) :
   C_GiBiImageGroup(oru64_Id, orc_ImagePath, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eIMAGE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index Index of data element in system definition
   \param[in]     oru64_Id    Unique ID
   \param[in]     of64_Width  Width of image view
   \param[in]     of64_Height Height of image view
   \param[in]     orc_Image   Image
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSdImageGroup::C_GiSdImageGroup(const int32_t & ors32_Index, const uint64_t & oru64_Id, const float64_t of64_Width,
                                   const float64_t of64_Height, const QPixmap & orc_Image,
                                   QGraphicsItem * const opc_Parent) :
   C_GiBiImageGroup(oru64_Id, of64_Width, of64_Height, orc_Image, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eIMAGE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdImageGroup::LoadData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size() > static_cast<uint32_t>(this->ms32_Index))
   {
      this->LoadBasicData(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images[static_cast<uint32_t>(this->ms32_Index)]);
   }
   //Object name for test
   this->setObjectName("Meta: Image");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdImageGroup::UpdateData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size() > static_cast<uint32_t>(this->ms32_Index))
   {
      m_UpdateData(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images[this->ms32_Index]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSdImageGroup::DeleteData(void)
{
   C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.erase(
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.begin() + ms32_Index);
}
