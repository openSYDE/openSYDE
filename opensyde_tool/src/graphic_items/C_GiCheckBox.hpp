//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics item for showing a checkbox with both states (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GICHECKBOX_H
#define C_GICHECKBOX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPixmap>

#include "C_GiRectPixmap.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiCheckBox :
   public C_GiRectPixmap
{
public:
   C_GiCheckBox(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiCheckBox();

   void SetChecked(const bool oq_Checked);
   bool IsChecked(void) const;

private:
   QPixmap mc_IconCheckBoxActive;

   bool mq_Checked;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
