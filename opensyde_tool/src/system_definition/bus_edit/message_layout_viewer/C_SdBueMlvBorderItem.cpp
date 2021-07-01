//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for border items of message layout viewer (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "constants.h"

#include "C_SdBueMlvBorderItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      orc_Title     Optional border title
   \param[in]      ou8_Index     Optional bit index
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvBorderItem::C_SdBueMlvBorderItem(const QString & orc_Title, const stw_types::uint8 ou8_Index,
                                           QGraphicsItem * const opc_Parent) :
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   C_SdBueMlvBaseItem(mc_STYLE_GUIDE_COLOR_10, mc_STYLE_GUIDE_COLOR_6, mc_STYLE_GUIDE_FONT_SEMIBOLD_16, "", true,
                      opc_Parent),
   mc_Title(orc_Title),
   mc_BackgroundColorInactive(mc_STYLE_GUIDE_COLOR_11),
   mc_FontColorInactive(mc_STYLE_GUIDE_COLOR_10)
{
   if (this->mc_Title != "")
   {
      this->SetText(this->mc_Title + QString::number(ou8_Index));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvBorderItem::~C_SdBueMlvBorderItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the index number for the text

   \param[in]  ou8_Index   Index number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvBorderItem::SetIndex(const stw_types::uint8 ou8_Index)
{
   if (this->mc_Title != "")
   {
      this->SetText(this->mc_Title + QString::number(ou8_Index));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the active state

   \param[in]  oq_Active   Flag if active state is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvBorderItem::SetActive(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->mc_DrawBackgroundColor = this->mc_BackgroundColorDefault;
      this->mc_DrawFontColor = this->mc_FontColorDefault;
   }
   else
   {
      this->mc_DrawBackgroundColor = this->mc_BackgroundColorInactive;
      this->mc_DrawFontColor = this->mc_FontColorInactive;
   }
   this->update();
}
