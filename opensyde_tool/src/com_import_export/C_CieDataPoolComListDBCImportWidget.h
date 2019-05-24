//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting a node of a DBC file import (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDATAPOOLCOMLISTDBCIMPORTWIDGET_H
#define C_CIEDATAPOOLCOMLISTDBCIMPORTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_CieDataPoolComListDBCImportWidget.h"
#include "C_OgePopUpDialog.h"
#include "C_CieConverter.h"

namespace Ui
{
class C_CieDataPoolComListDBCImportWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieDataPoolComListDBCImportWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDataPoolComListDBCImportWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                stw_opensyde_gui_logic::C_CieConverter::C_CIECommDefinition & orc_CIECommDefinition);
   virtual ~C_CieDataPoolComListDBCImportWidget();

   void InitStaticNames(void);

   stw_types::sint32 GetNodeSelection(void) const;

protected:
   // empty

private:
   //Avoid call
   C_CieDataPoolComListDBCImportWidget & operator =(const C_CieDataPoolComListDBCImportWidget &);
   C_CieDataPoolComListDBCImportWidget(const C_CieDataPoolComListDBCImportWidget &);

   void m_DisplayNodeInfo(void);

   void m_OkClicked(void) const;
   void m_CancelClicked(void) const;

   void m_FillUpComboBox(void);

   Ui::C_CieDataPoolComListDBCImportWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_logic::C_CieConverter::C_CIECommDefinition & mrc_CIECommDefinition;

   stw_types::sint32 ms32_NodeIndex;
};
}

#endif
