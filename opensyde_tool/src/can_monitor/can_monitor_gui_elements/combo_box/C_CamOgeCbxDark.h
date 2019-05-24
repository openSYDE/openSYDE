//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dark combo-box for settings bar. (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGECBXDARK_H
#define C_CAMOGECBXDARK_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeCbxDark :
   public C_OgeCbxToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeCbxDark(QWidget * const opc_Parent = NULL);
   ~C_CamOgeCbxDark(void);

   //Avoid call
   C_CamOgeCbxDark(const C_CamOgeCbxDark &);
   C_CamOgeCbxDark & operator =(const C_CamOgeCbxDark &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
