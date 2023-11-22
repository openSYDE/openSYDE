//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup widget where user can select which DBC node corresponds to which openSYDE node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDBCIMPORTNODEASSIGNMENTWIDGET_HPP
#define C_CIEDBCIMPORTNODEASSIGNMENTWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"
#include "C_CieConverter.hpp"
#include "C_CieDbcImportNodeAssignmentItemWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CieDbcImportNodeAssignmentWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieDbcImportNodeAssignmentWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDbcImportNodeAssignmentWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                               const QString & orc_FilePath, const uint32_t ou32_BusIndex,
                                               const stw::opensyde_gui_logic::C_CieConverter::C_CieCommDefinition & orc_CieCommDef);
   ~C_CieDbcImportNodeAssignmentWidget(void) override;

   void InitStaticNames(void) const;
   std::vector<C_CieDbcOsyNodeAssignment> GetNodeAssignments(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CieDbcImportNodeAssignmentWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   std::vector<stw::opensyde_gui::C_CieDbcImportNodeAssignmentItemWidget *> mc_Entries;
   std::vector<uint32_t> mc_NodeIndexes;
   std::vector<uint32_t> mc_InterfaceIndexes;
   std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> mc_UnmappedMessages;

   static const uint8_t mhu8_INDEX_TRANSMIT;
   static const uint8_t mhu8_INDEX_RECEIVE;

   void m_OkClicked(void);
   void m_CancelClicked(void) const;

   void m_InitNodes(const uint32_t ou32_BusIndex,
                    const stw::opensyde_gui_logic::C_CieConverter::C_CieCommDefinition & orc_CieCommDef);
   void m_OnUnmappedCbxIndexChanged(const int32_t os32_Index);
   void m_UpdateComboboxEntries(const uint32_t ou32_Index, const bool oq_Enable,
                                const C_CieDbcImportNodeAssignmentItemWidget * const opc_Sender);

   //Avoid call
   C_CieDbcImportNodeAssignmentWidget(const C_CieDbcImportNodeAssignmentWidget &);
   C_CieDbcImportNodeAssignmentWidget & operator =(const C_CieDbcImportNodeAssignmentWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
