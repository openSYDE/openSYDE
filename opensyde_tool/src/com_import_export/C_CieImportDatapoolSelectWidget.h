//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting a datapool while DBC/EDS/DCF file import.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEIMPORTDATAPOOLSELECTWIDGET_H
#define C_CIEIMPORTDATAPOOLSELECTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CieImportDatapoolSelectWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieImportDatapoolSelectWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieImportDatapoolSelectWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                            const stw_types::uint32 ou32_NodeIndex,
                                            const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ComProtocolType);
   ~C_CieImportDatapoolSelectWidget(void);

   void InitStaticNames(void) const;
   stw_types::uint32 GetSelectedDatapoolIndex(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CieImportDatapoolSelectWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_NodeIndex;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ComProtocolType;

   void m_OkClicked(void);
   void m_CancelClicked(void) const;
   void m_InitComboBox(void) const;

   //Avoid call
   C_CieImportDatapoolSelectWidget(const C_CieImportDatapoolSelectWidget &);
   C_CieImportDatapoolSelectWidget & operator =(const C_CieImportDatapoolSelectWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
