//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group box for navigation tab style (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXNAVIGATIONTAB_H
#define C_OGEGBXNAVIGATIONTAB_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxNavigationTab :
   public QGroupBox
{
   Q_OBJECT

public:
   explicit C_OgeGbxNavigationTab(QWidget * const opc_Parent = NULL);

   void SetIncludeLeftBorder(const bool oq_Include);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
