//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom white combo box (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGECBXWHITE_HPP
#define C_CAMOGECBXWHITE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeCbxWhite :
   public C_OgeCbxToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeCbxWhite(QWidget * const opc_Parent = NULL);
   virtual ~C_CamOgeCbxWhite(void);

private:
   //Avoid call
   C_CamOgeCbxWhite(const C_CamOgeCbxWhite &);
   C_CamOgeCbxWhite & operator =(const C_CamOgeCbxWhite &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
