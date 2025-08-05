//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image (implementation)

   detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GiBiImageGroup.hpp"
#include "C_PuiSdDataElement.hpp"
#include "C_PuiBsImage.hpp"
#include "gitypes.hpp"

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

   \param[in]     oru64_Id      Unique ID
   \param[in]     orc_ImagePath File path of image
   \param[in,out] opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiImageGroup::C_GiBiImageGroup(const uint64_t & oru64_Id, const QString & orc_ImagePath,
                                   QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiImageGroupWithoutData(oru64_Id, orc_ImagePath, false, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]      oru64_Id      Unique ID
   \param[in]      of64_Width    Width of image view
   \param[in]      of64_Height   Height of image view
   \param[in]      orc_Image     Image
   \param[in]      orc_Format    Image format
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiImageGroup::C_GiBiImageGroup(const uint64_t & oru64_Id, const float64_t of64_Width, const float64_t of64_Height,
                                   const QPixmap & orc_Image, const QByteArray & orc_Format,
                                   QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiImageGroupWithoutData(oru64_Id, of64_Width, of64_Height, orc_Image, orc_Format, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiBiImageGroup::type(void) const
{
   return ms32_GRAPHICS_ITEM_IMAGE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiImageGroup::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiImageGroupWithoutData::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update internal data

   \param[in]  orc_Data    Image data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiImageGroup::m_UpdateData(C_PuiBsImage & orc_Data) const
{
   this->UpdateBasicData(orc_Data);

   orc_Data.c_UiImagePixmap = this->GetImage();
   orc_Data.c_UiImageFormat = this->GetImageFormat();
}
