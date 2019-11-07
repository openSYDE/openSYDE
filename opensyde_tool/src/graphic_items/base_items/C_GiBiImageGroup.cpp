//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image (implementation)

   detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GiBiImageGroup.h"
#include "C_PuiSdDataElement.h"
#include "C_PuiBsImage.h"
#include "gitypes.h"

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
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_ID      Unique ID
   \param[in]     orc_ImagePath File path of image
   \param[in,out] opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiImageGroup::C_GiBiImageGroup(const uint64 & oru64_ID, const QString & orc_ImagePath,
                                   QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiImageGroupWithoutData(oru64_ID, orc_ImagePath, false, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_ID    Unique ID
   \param[in]     of64_Width  Width of image view
   \param[in]     of64_Height Height of image view
   \param[in]     orc_Image   Image
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiImageGroup::C_GiBiImageGroup(const uint64 & oru64_ID, const float64 of64_Width, const float64 of64_Height,
                                   const QPixmap & orc_Image, QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiImageGroupWithoutData(oru64_ID, of64_Width, of64_Height, orc_Image,
                             opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiBiImageGroup::type(void) const
{
   return msn_GRAPHICS_ITEM_IMAGE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiImageGroup::SetZValueCustom(const float64 of64_ZValue)
{
   C_GiImageGroupWithoutData::SetZValueCustom(of64_ZValue);
   //Apply to data
   this->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update internal data

   \param[in] orc_Data Image data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiImageGroup::m_UpdateData(C_PuiBsImage & orc_Data) const
{
   this->UpdateBasicData(orc_Data);

   orc_Data.c_UIImagePixmap = this->GetImage();
}
