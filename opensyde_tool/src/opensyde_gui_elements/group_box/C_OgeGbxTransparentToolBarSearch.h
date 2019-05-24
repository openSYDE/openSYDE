//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom group box transparent for toolbar search (header)

   Custom group box (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXTRANSPARENTTOOLBARSEARCH_H
#define C_OGEGBXTRANSPARENTTOOLBARSEARCH_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxTransparentToolBarSearch :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxTransparentToolBarSearch(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
