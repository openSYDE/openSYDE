//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box for navigation design
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXNAVIGATIONSECTION2_HPP
#define C_OGEGBXNAVIGATIONSECTION2_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxNavigationSection2 :
   public QGroupBox
{
   Q_OBJECT

public:
   explicit C_OgeGbxNavigationSection2(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
