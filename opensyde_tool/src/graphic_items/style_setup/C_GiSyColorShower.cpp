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
#include "precomp_headers.hpp"

#include "C_GiSyColorShower.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

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
   ms32_Alpha(0)
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

   \param[in]   os32_Hue     new hue value
   \param[in]   os32_Sat     new sat value
   \param[in]   os32_Value   new value value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorShower::SetHsv(const int32_t os32_Hue, const int32_t os32_Sat, const int32_t os32_Value)
{
   if ((os32_Hue >= -1) && (os32_Sat <= 255) && (os32_Value <= 255))
   {
      QColor c_Color;
      c_Color.setHsv(os32_Hue, os32_Sat, os32_Value);
      this->mc_CurrentRgbaColor = c_Color.rgba();
      this->mc_CurrentRgbaColor.setAlpha(this->ms32_Alpha);
      Q_EMIT (this->SigChangeColor(CurrentColor()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the rgba value to color shower

   \param[in, out]   ou32_Rgb   new rgb value

   \return
   The current color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyColorShower::SetRgb(const uint32_t ou32_Rgb)
{
   this->mc_CurrentRgbaColor.setRgba(qRgba(qRed(ou32_Rgb),
                                           qGreen(ou32_Rgb),
                                           qBlue(ou32_Rgb),
                                           this->ms32_Alpha));
   Q_EMIT (this->SigChangeColor(CurrentColor()));
   return this->mc_CurrentRgbaColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return current color of color shower

   \return
   QColor   Current color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyColorShower::CurrentColor(void) const
{
   return this->mc_CurrentRgbaColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new current color

   \param[in]   orc_Color   Reference of new current color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorShower::SetCurrentColor(const QColor & orc_Color)
{
   this->mc_CurrentRgbaColor = orc_Color;
   Q_EMIT (this->SigChangeColor(CurrentColor()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set current alpha of current color

   \param[in]   os32_Alpha   New alpha value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorShower::SetCurrentAlpha(const int32_t os32_Alpha)
{
   this->ms32_Alpha = os32_Alpha;
}
