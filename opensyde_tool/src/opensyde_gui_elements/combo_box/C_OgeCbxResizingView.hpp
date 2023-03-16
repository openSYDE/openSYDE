//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base combo box class with resizing view functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXRESIZINGVIEW_H
#define C_OGECBXRESIZINGVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QComboBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxResizingView :
   public QComboBox
{
public:
   C_OgeCbxResizingView(QWidget * const opc_Parent = NULL);

   void ResizeViewToContents(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
