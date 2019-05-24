//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handling all node selector widgets in a kind of list (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUENODESELECTORCHECKBOXLISTWIDGET_H
#define C_SDBUENODESELECTORCHECKBOXLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QIcon>
#include <vector>

#include "stwtypes.h"

#include "C_SdBueNodeSelectorCheckBoxItemWidget.h"
#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueNodeSelectorCheckBoxListWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueNodeSelectorCheckBoxListWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueNodeSelectorCheckBoxListWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueNodeSelectorCheckBoxListWidget();

   void AddNodes(const std::vector<QString> & orc_Names, const std::vector<stw_types::uint32> & orc_Indexes,
                 const std::vector<stw_types::uint32> & orc_SubIndexes);
   void CheckNodes(const std::vector<stw_types::uint32> & orc_Indexes,
                   const std::vector<stw_types::uint32> & orc_SubIndexes) const;
   void CheckSpecificNode(const stw_types::uint32 ou32_Index, const stw_types::uint32 ou32_SubIndex,
                          const bool oq_Checked) const;
   void SetProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol) const;
   void Refresh(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                       const bool oq_Checked);
   void SigComImport(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex);

private:
   //Avoid call
   C_SdBueNodeSelectorCheckBoxListWidget(const C_SdBueNodeSelectorCheckBoxListWidget &);
   C_SdBueNodeSelectorCheckBoxListWidget & operator =(const C_SdBueNodeSelectorCheckBoxListWidget &);

   void m_AddNode(const QString & orc_Name, const stw_types::uint32 ou32_Index, const stw_types::uint32 ou32_SubIndex);
   void m_ScrollBarRangeChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;

   Ui::C_SdBueNodeSelectorCheckBoxListWidget * mpc_Ui;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
