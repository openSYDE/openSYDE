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
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyColorShower :
   public QPushButton
{
   Q_OBJECT

public:
   C_GiSyColorShower(QWidget * const opc_Parent = NULL);
   ~C_GiSyColorShower(void);

   // things that don't emit signals
   void SetHsv(const stw_types::sintn osn_Hue, const stw_types::sintn osn_Sat, const stw_types::sintn osn_Value);
   QColor SetRgb(const stw_types::uintn oun_Rgb); // see QRgb
   QColor CurrentColor(void) const;
   void SetCurrentColor(const QColor & orc_Color);
   void SetCurrentAlpha(const stw_types::sintn osn_Alpha);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigChangeColor(QColor oc_Color);

private:
   QColor mc_CurrentRGBAColor;
   stw_types::sintn msn_Alpha;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
