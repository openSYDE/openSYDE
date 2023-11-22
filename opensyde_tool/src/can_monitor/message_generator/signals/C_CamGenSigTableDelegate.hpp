//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for message generator signals table (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENSIGTABLEDELEGATE_HPP
#define C_CAMGENSIGTABLEDELEGATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_CamTblDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenSigTableDelegate :
   public C_CamTblDelegate
{
   Q_OBJECT

public:
   C_CamGenSigTableDelegate(QObject * const opc_Parent = NULL);

   void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigAfterSetEditorData(QLineEdit * const opc_Widget) const;

protected:
   stw::opensyde_gui_elements::C_OgeCbxTableBase * m_CreateComboBox(QWidget * const opc_Parent) const override;

private:
   void m_AfterSetEditorData(QLineEdit * const opc_Widget) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
