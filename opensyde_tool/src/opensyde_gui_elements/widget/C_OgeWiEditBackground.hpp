//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the edit background (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIEDITBACKGROUND_HPP
#define C_OGEWIEDITBACKGROUND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiEditBackground :
   public QWidget
{
   Q_OBJECT

public:
   C_OgeWiEditBackground(QWidget * const opc_Parent = NULL);

   void SetEditBackground(const bool oq_Active);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
