//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic table combo box interaction field (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGECBXTABLE_HPP
#define C_CAMOGECBXTABLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeCbxTableBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeCbxTable :
   public C_OgeCbxTableBase
{
   Q_OBJECT

public:
   C_CamOgeCbxTable(QWidget * const opc_Parent = NULL);
   virtual ~C_CamOgeCbxTable(void);

private:
   //Avoid call
   C_CamOgeCbxTable(const C_CamOgeCbxTable &);
   C_CamOgeCbxTable & operator =(const C_CamOgeCbxTable &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
