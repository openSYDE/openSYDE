//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple widget with a border (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIBORDER_H
#define C_OGEWIBORDER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiBorder :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiBorder(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
