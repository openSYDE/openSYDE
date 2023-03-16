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

#include "stwtypes.hpp"

#include "C_SdBueNodeSelectorCheckBoxItemWidget.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueNodeSelectorCheckBoxListWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueNodeSelectorCheckBoxListWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueNodeSelectorCheckBoxListWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueNodeSelectorCheckBoxListWidget() override;

   void AddNodes(const std::vector<QString> & orc_Names, const std::vector<uint32_t> & orc_Indexes,
                 const std::vector<uint32_t> & orc_SubIndexes);
   void CheckNodes(const std::vector<uint32_t> & orc_Indexes, const std::vector<uint32_t> & orc_SubIndexes) const;
   void CheckSpecificNode(const uint32_t ou32_Index, const uint32_t ou32_SubIndex, const bool oq_Checked) const;
   void SetSpecificNodeAsManager(const uint32_t ou32_Index, const uint32_t ou32_SubIndex, const bool oq_Manager) const;
   void SetSpecificNodeAsDevice(const uint32_t ou32_Index, const uint32_t ou32_SubIndex, const bool oq_Device,
                                const stw::opensyde_core::C_OscCanInterfaceId * const opc_DeviceId,
                                const uint32_t ou32_ManagerNodeIndex = 0U,
                                const uint32_t ou32_ManagerIntfIndex = 0U) const;
   void SetProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol) const;
   void Refresh(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex, const bool oq_Checked);
   void SigSwitchToCoManager(const uint32_t ou32_NodeIndex, const QString & orc_NodeName,
                             const uint8_t ou8_InterfaceNumber) const;

private:
   //Avoid call
   C_SdBueNodeSelectorCheckBoxListWidget(const C_SdBueNodeSelectorCheckBoxListWidget &);
   C_SdBueNodeSelectorCheckBoxListWidget & operator =(const C_SdBueNodeSelectorCheckBoxListWidget &) &;

   void m_AddNode(const QString & orc_Name, const uint32_t ou32_Index, const uint32_t ou32_SubIndex);
   void m_ScrollBarRangeChanged(const int32_t os32_Min, const int32_t os32_Max) const;
   C_SdBueNodeSelectorCheckBoxItemWidget * m_GetItemWidget(const uint32_t ou32_Index,
                                                           const uint32_t ou32_SubIndex) const;

   Ui::C_SdBueNodeSelectorCheckBoxListWidget * mpc_Ui;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
