//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box styled for smaller widget usage (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGECBXTABLESMALL_H
#define C_CAMOGECBXTABLESMALL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxTableBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeCbxTableSmall :
   public C_OgeCbxTableBase
{
   Q_OBJECT

public:
   explicit C_CamOgeCbxTableSmall(QWidget * const opc_Parent = NULL);
   ~C_CamOgeCbxTableSmall(void);

private:
   //Avoid call
   C_CamOgeCbxTableSmall(const C_CamOgeCbxTableSmall &);
   C_CamOgeCbxTableSmall & operator =(const C_CamOgeCbxTableSmall &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
