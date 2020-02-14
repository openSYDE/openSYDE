//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard color button in color picker (header)

   See .cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBCOLOR_H
#define C_OGEPUBCOLOR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>

#include "C_OgeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubColor :
   public QPushButton,
   public C_OgeToolTipBase
{
   Q_OBJECT

public:
   C_OgePubColor(QWidget * const opc_Parent = NULL);
   void SetColors(const QColor & orc_BackgroundColor, const QColor & orc_TopBorderColor,
                  const QColor & orc_RestBorderColor);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigHideOtherToolTips(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
