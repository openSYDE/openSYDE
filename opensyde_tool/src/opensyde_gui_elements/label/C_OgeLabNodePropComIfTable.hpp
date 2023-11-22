//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for Node Prop Com If Table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABNODEPROPCOMIFTABLE_HPP
#define C_OGELABNODEPROPCOMIFTABLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabNodePropComIfTable :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabNodePropComIfTable(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
