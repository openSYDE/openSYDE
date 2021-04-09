//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GiSyColorShower.h"

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
/*! \brief  Default constructor

   Set up the color shower widget with all its functions.

   \param[in,out]   opc_Parent Pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorShower::C_GiSyColorShower(QWidget * const opc_Parent) :
   QPushButton(opc_Parent),
   msn_Alpha(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorShower::~C_GiSyColorShower(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set a hue, sat and value to color shower

   \param[in]   osn_Hue     new hue value
   \param[in]   osn_Sat     new sat value
   \param[in]   osn_Value   new value value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorShower::SetHsv(const sintn osn_Hue, const sintn osn_Sat, const sintn osn_Value)
{
   if ((osn_Hue < -1) || (osn_Sat > 255) || (osn_Value > 255))
   {
      return;
   }
   QColor c_Color;
   c_Color.setHsv(osn_Hue, osn_Sat, osn_Value);
   this->mc_CurrentRGBAColor = c_Color.rgba();
   this->mc_CurrentRGBAColor.setAlpha(this->msn_Alpha);
   Q_EMIT (this->SigChangeColor(CurrentColor()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the rgba value to color shower

   \param[in, out]   oun_Rgb   new rgb value

   \return
   The current color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyColorShower::SetRgb(const uintn oun_Rgb)
{
   this->mc_CurrentRGBAColor.setRgba(qRgba(qRed(oun_Rgb),
                                           qGreen(oun_Rgb),
                                           qBlue(oun_Rgb),
                                           this->msn_Alpha));
   Q_EMIT (this->SigChangeColor(CurrentColor()));
   return this->mc_CurrentRGBAColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return current color of color shower

   \return
   QColor   Current color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyColorShower::CurrentColor(void) const
{
   return this->mc_CurrentRGBAColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new current color

   \param[in]   orc_Color   Reference of new current color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorShower::SetCurrentColor(QColor const & orc_Color)
{
   this->mc_CurrentRGBAColor = orc_Color;
   Q_EMIT (this->SigChangeColor(CurrentColor()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set current alpha of current color

   \param[in]   osn_Alpha   New alpha value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorShower::SetCurrentAlpha(const sintn osn_Alpha)
{
   this->msn_Alpha = osn_Alpha;
}
