//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Item for list of DBC nodes, its messages and the selection combobox for corresponding topology node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDBCIMPORTNODEASSIGNMENTITEMWIDGET_H
#define C_CIEDBCIMPORTNODEASSIGNMENTITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_CieConverter.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CieDbcImportNodeAssignmentItemWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
// class to put together
class C_CieDbcOsyNodeAssignment
{
public:
   stw_opensyde_gui_logic::C_CieConverter::C_CIENode c_CieNode;
   stw_types::sint32 s32_AssignedOsyNodeIndex;      // index of assigned topology node (-1 if not assigned)
   stw_types::sint32 s32_AssignedOsyInterfaceIndex; // index of assigned topology node interface (-1 if not assigned)
};

class C_CieDbcImportNodeAssignmentItemWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDbcImportNodeAssignmentItemWidget(
      const stw_opensyde_gui_logic::C_CieConverter::C_CIENode & orc_DbcNode,
      const std::vector<QString> & orc_OsyNodeNames, const std::vector<stw_types::uint32> & orc_NodeIndexes,
      const std::vector<stw_types::uint32> & orc_InterfaceIndexes, QWidget * const opc_Parent = NULL);
   ~C_CieDbcImportNodeAssignmentItemWidget(void);

   C_CieDbcOsyNodeAssignment GetNodeAssignment(void) const;
   bool IsAssigned(void) const;

   void UpdateComboboxEntries(const stw_types::uint32 ou32_Index, const bool oq_Enable) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdateOtherComboboxes(const stw_types::uint32 ou32_Index, const bool oq_Enable,
                                 C_CieDbcImportNodeAssignmentItemWidget * const opc_Sender);

private:
   Ui::C_CieDbcImportNodeAssignmentItemWidget * mpc_Ui;
   const stw_opensyde_gui_logic::C_CieConverter::C_CIENode & mrc_DbcNode;
   const std::vector<stw_types::uint32> mc_NodeIndexes;
   const std::vector<stw_types::uint32> mc_InterfaceIndexes;
   stw_types::sint32 ms32_PreviousCbxIndex;

   void m_OnComboboxIndexChanged(const stw_types::sint32 os32_Index);

   //Avoid call
   C_CieDbcImportNodeAssignmentItemWidget(const C_CieDbcImportNodeAssignmentItemWidget &);
   C_CieDbcImportNodeAssignmentItemWidget & operator =(const C_CieDbcImportNodeAssignmentItemWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
