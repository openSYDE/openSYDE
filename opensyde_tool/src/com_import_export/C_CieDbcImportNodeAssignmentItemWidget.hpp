//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Item for list of DBC nodes, its messages and the selection combobox for corresponding topology node
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDBCIMPORTNODEASSIGNMENTITEMWIDGET_HPP
#define C_CIEDBCIMPORTNODEASSIGNMENTITEMWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_CieConverter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CieDbcImportNodeAssignmentItemWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
// class to put together
class C_CieDbcOsyNodeAssignment
{
public:
   stw::opensyde_gui_logic::C_CieConverter::C_CieNode c_CieNode;
   int32_t s32_AssignedOsyNodeIndex;      // index of assigned topology node (-1 if not assigned)
   int32_t s32_AssignedOsyInterfaceIndex; // index of assigned topology node interface (-1 if not assigned)
};

class C_CieDbcImportNodeAssignmentItemWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDbcImportNodeAssignmentItemWidget(
      const stw::opensyde_gui_logic::C_CieConverter::C_CieNode & orc_DbcNode,
      const std::vector<QString> & orc_OsyNodeNames, const std::vector<uint32_t> & orc_NodeIndexes,
      const std::vector<uint32_t> & orc_InterfaceIndexes, QWidget * const opc_Parent = NULL);
   ~C_CieDbcImportNodeAssignmentItemWidget(void) override;

   C_CieDbcOsyNodeAssignment GetNodeAssignment(void) const;
   bool IsAssigned(void) const;

   void UpdateComboboxEntries(const uint32_t ou32_Index, const bool oq_Enable) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdateOtherComboboxes(const uint32_t ou32_Index, const bool oq_Enable,
                                 C_CieDbcImportNodeAssignmentItemWidget * const opc_Sender);

private:
   Ui::C_CieDbcImportNodeAssignmentItemWidget * mpc_Ui;
   const stw::opensyde_gui_logic::C_CieConverter::C_CieNode & mrc_DbcNode;
   const std::vector<uint32_t> mc_NodeIndexes;
   const std::vector<uint32_t> mc_InterfaceIndexes;
   int32_t ms32_PreviousCbxIndex;

   void m_OnComboboxIndexChanged(const int32_t os32_Index);

   //Avoid call
   C_CieDbcImportNodeAssignmentItemWidget(const C_CieDbcImportNodeAssignmentItemWidget &);
   C_CieDbcImportNodeAssignmentItemWidget & operator =(const C_CieDbcImportNodeAssignmentItemWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
