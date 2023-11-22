//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard color button in color picker (header)

   See .cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBCOLOR_HPP
#define C_OGEPUBCOLOR_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>

#include "C_OgeToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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
   bool event(QEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
