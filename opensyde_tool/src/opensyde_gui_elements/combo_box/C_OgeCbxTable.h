//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box for table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXTABLE_H
#define C_OGECBXTABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxResizingView.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxTable :
   public C_OgeCbxResizingView
{
   Q_OBJECT

public:
   C_OgeCbxTable(QWidget * const opc_Parent = NULL);
   ~C_OgeCbxTable(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_OgeCbxTable(const C_OgeCbxTable &);
   C_OgeCbxTable & operator =(const C_OgeCbxTable &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
