//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget of tool box (header)

   Tab widget of tool box (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETAWTOOLBOX_HPP
#define C_OGETAWTOOLBOX_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QTabWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTawToolBox :
   public QTabWidget
{
   Q_OBJECT

public:
   C_OgeTawToolBox(QWidget * const opc_Parent = NULL);
   ~C_OgeTawToolBox(void) override;

private:
   //Avoid call
   C_OgeTawToolBox(const C_OgeTawToolBox &);
   C_OgeTawToolBox & operator =(const C_OgeTawToolBox &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
