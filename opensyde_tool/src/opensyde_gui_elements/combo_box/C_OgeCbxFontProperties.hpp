//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box for font (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXFONTPROPERTIES_H
#define C_OGECBXFONTPROPERTIES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFontComboBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxFontProperties :
   public QFontComboBox
{
   Q_OBJECT

public:
   C_OgeCbxFontProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
