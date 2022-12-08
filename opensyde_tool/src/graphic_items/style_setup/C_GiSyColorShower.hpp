//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_COLORSHOWER_H
#define C_COLORSHOWER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyColorShower :
   public QPushButton
{
   Q_OBJECT

public:
   C_GiSyColorShower(QWidget * const opc_Parent = NULL);
   ~C_GiSyColorShower(void) override;

   // things that don't emit signals
   void SetHsv(const int32_t os32_Hue, const int32_t os32_Sat, const int32_t os32_Value);
   QColor SetRgb(const uint32_t ou32_Rgb); // see QRgb
   QColor CurrentColor(void) const;
   void SetCurrentColor(const QColor & orc_Color);
   void SetCurrentAlpha(const int32_t os32_Alpha);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigChangeColor(QColor oc_Color);

private:
   QColor mc_CurrentRgbaColor;
   int32_t ms32_Alpha;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
