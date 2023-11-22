//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box for navigation heading background
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXNAVIGATIONHEADINGBACKGROUND_HPP
#define C_OGEGBXNAVIGATIONHEADINGBACKGROUND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxNavigationHeadingBackground :
   public QGroupBox
{
   Q_OBJECT

public:
   explicit C_OgeGbxNavigationHeadingBackground(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
