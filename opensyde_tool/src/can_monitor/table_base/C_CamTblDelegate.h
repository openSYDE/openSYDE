//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate (interaction) component base for CAN monitor table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMTBLDELEGATE_H
#define C_CAMTBLDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamTblDelegate :
   public C_TblDelegate
{
   Q_OBJECT

public:
   C_CamTblDelegate(QObject * const opc_Parent = NULL);

protected:
   virtual stw_opensyde_gui_elements::C_OgeCbxTableBase * m_CreateComboBox(QWidget * const opc_Parent) const override;
   virtual stw_opensyde_gui_elements::C_OgeCbxMultiSelect * m_CreateMultiSelectComboBox(QWidget * const opc_Parent)
   const override;
   virtual stw_opensyde_gui::C_TblEditLineEditBase * m_CreateLineEdit(QWidget * const opc_Parent) const override;
   virtual stw_opensyde_gui_elements::C_OgeWiSpinBoxGroup * m_CreateSpinBox(QWidget * const opc_Parent) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
