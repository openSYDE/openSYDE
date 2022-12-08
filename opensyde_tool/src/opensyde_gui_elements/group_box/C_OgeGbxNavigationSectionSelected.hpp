//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box style for selectable navigation section
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXNAVIGATIONSECTIONSELECTED_H
#define C_OGEGBXNAVIGATIONSECTIONSELECTED_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxNavigationSectionSelected :
   public QGroupBox
{
   Q_OBJECT

public:
   explicit C_OgeGbxNavigationSectionSelected(QWidget * const opc_Parent = NULL);

   void SetActive(const bool oq_Active);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
