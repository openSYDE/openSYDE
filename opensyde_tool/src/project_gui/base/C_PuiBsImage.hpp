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

#include "C_PuiBsBox.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsImage :
   public C_PuiBsBox
{
public:
   C_PuiBsImage();

   void CalcHash(uint32_t & oru32_HashValue) const override;

   QPixmap c_UiImagePixmap;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
