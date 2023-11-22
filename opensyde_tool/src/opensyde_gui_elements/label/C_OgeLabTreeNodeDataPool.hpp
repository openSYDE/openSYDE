//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for tree node data pool (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABTREENODEDATAPOOL_HPP
#define C_OGELABTREENODEDATAPOOL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabTreeNodeDataPool :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabTreeNodeDataPool(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
