//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box for miscellaneous navigation section (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXNAVIGATIONSECTION_HPP
#define C_OGEGBXNAVIGATIONSECTION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxNavigationSection :
   public QGroupBox
{
   Q_OBJECT

public:
   explicit C_OgeGbxNavigationSection(QWidget * const opc_Parent = NULL);

   void SetSpecialBackground(const bool oq_Active);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
