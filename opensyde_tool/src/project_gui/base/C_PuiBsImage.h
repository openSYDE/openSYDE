//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI image data: stores UI information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSIMAGE_H
#define C_PUIBSIMAGE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPixmap>

#include "C_PuiBsBox.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsImage :
   public C_PuiBsBox
{
public:
   C_PuiBsImage();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;

   QPixmap c_UIImagePixmap;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
