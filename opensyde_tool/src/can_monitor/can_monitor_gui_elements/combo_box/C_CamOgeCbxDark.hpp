//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dark combo-box for settings bar. (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGECBXDARK_HPP
#define C_CAMOGECBXDARK_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeCbxDark :
   public C_OgeCbxToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeCbxDark(QWidget * const opc_Parent = NULL);
   ~C_CamOgeCbxDark(void) override;

private:
   //Avoid call
   C_CamOgeCbxDark(const C_CamOgeCbxDark &);                //not implemented -> prevent copying
   C_CamOgeCbxDark & operator =(const C_CamOgeCbxDark &) &; //not implemented -> prevent assignment
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
