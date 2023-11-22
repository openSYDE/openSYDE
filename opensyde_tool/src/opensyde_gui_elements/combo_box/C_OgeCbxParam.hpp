//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box for param tree (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXPARAM_HPP
#define C_OGECBXPARAM_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QComboBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxParam :
   public QComboBox
{
   Q_OBJECT

public:
   explicit C_OgeCbxParam(QWidget * const opc_Parent = NULL);
   ~C_OgeCbxParam(void) override;

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   //Avoid call
   C_OgeCbxParam(const C_OgeCbxParam &);
   C_OgeCbxParam & operator =(const C_OgeCbxParam &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
