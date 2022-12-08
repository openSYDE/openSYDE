//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree delegate for HALC use case configuration.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGTREEDELEGATE_H
#define C_SDNDEHALCCONFIGTREEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigTreeDelegate :
   public stw::opensyde_gui::C_TblDelegate
{
public:
   C_SdNdeHalcConfigTreeDelegate(QObject * const opc_Parent = NULL);

protected:
   stw::opensyde_gui_elements::C_OgeCbxTableBase * m_CreateComboBox(QWidget * const opc_Parent) const override;
   stw::opensyde_gui_elements::C_OgeCbxMultiSelect * m_CreateMultiSelectComboBox(QWidget * const opc_Parent)
   const override;
   stw::opensyde_gui::C_TblEditLineEditBase * m_CreateLineEdit(QWidget * const opc_Parent) const override;
   stw::opensyde_gui_elements::C_OgeWiSpinBoxGroup * m_CreateSpinBox(QWidget * const opc_Parent) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
