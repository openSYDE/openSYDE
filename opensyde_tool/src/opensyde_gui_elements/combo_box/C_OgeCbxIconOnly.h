//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box with icons only (header)

   Custom combo box (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXICONONLY_H
#define C_OGECBXICONONLY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeCbxToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxIconOnly :
   public C_OgeCbxToolTipBase
{
   Q_OBJECT

public:
   C_OgeCbxIconOnly(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeCbxIconOnly(void);

private:
   //Avoid call
   C_OgeCbxIconOnly(const C_OgeCbxIconOnly &);
   C_OgeCbxIconOnly & operator =(const C_OgeCbxIconOnly &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
