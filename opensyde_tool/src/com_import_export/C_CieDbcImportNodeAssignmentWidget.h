//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup widget where user can select which DBC node corresponds to which openSYDE node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDBCIMPORTNODEASSIGNMENTWIDGET_H
#define C_CIEDBCIMPORTNODEASSIGNMENTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include "C_CieConverter.h"
#include "C_CieDbcImportNodeAssignmentItemWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CieDbcImportNodeAssignmentWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieDbcImportNodeAssignmentWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDbcImportNodeAssignmentWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                               const QString & orc_FilePath, const stw_types::uint32 ou32_BusIndex,
                                               const stw_opensyde_gui_logic::C_CieConverter::C_CIECommDefinition & orc_CIECommDef);
   ~C_CieDbcImportNodeAssignmentWidget(void);

   void InitStaticNames(void) const;
   std::vector<C_CieDbcOsyNodeAssignment> GetNodeAssignments(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CieDbcImportNodeAssignmentWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   std::vector<stw_opensyde_gui::C_CieDbcImportNodeAssignmentItemWidget *> mc_Entries;
   std::vector<stw_types::uint32> mc_NodeIndexes;
   std::vector<stw_types::uint32> mc_InterfaceIndexes;
   std::vector<stw_opensyde_gui_logic::C_CieConverter::C_CIENodeMessage> mc_UnmappedMessages;

   static const stw_types::uint8 mhu8_INDEX_TRANSMIT;
   static const stw_types::uint8 mhu8_INDEX_RECEIVE;

   void m_OkClicked(void);
   void m_CancelClicked(void) const;

   void m_InitNodes(const stw_types::uint32 ou32_BusIndex,
                    const stw_opensyde_gui_logic::C_CieConverter::C_CIECommDefinition & orc_CIECommDef);
   void m_OnUnmappedCbxIndexChanged(const stw_types::sint32 os32_Index);
   void m_UpdateComboboxEntries(const stw_types::uint32 ou32_Index, const bool oq_Enable,
                                const C_CieDbcImportNodeAssignmentItemWidget * const opc_Sender);

   //Avoid call
   C_CieDbcImportNodeAssignmentWidget(const C_CieDbcImportNodeAssignmentWidget &);
   C_CieDbcImportNodeAssignmentWidget & operator =(const C_CieDbcImportNodeAssignmentWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
