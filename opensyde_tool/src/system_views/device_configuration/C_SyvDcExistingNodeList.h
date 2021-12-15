//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for all existing node entries (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCEXISTINGNODELIST_H
#define C_SYVDCEXISTINGNODELIST_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QListWidget>
#include "stwtypes.h"
#include "C_SyvDcSequences.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDcExistingNodeList :
   public QListWidget
{
   Q_OBJECT

public:
   C_SyvDcExistingNodeList(QWidget * const opc_Parent = NULL);

   stw_types::sint32 SetView(const stw_types::uint32 ou32_Index, const bool oq_ShowAssignment);
   void ConnectSerialNumber(const stw_types::uint32 ou32_NodeIndex,
                            const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber,
                            const std::map<stw_types::uint8,
                                           stw_opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
   const;
   void DisconnectSerialNumber(const stw_types::uint32 ou32_NodeIndex,
                               const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber) const;
   stw_types::uint32 GetCommunicatingNodeCount(void) const;
   stw_types::uint32 GetAssignmentCount(void) const;
   std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation> GetConfigs(void) const;

   void StartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid) const;
   void StopDrag(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConnect(const stw_types::uint32 ou32_NodeIndex,
                   const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber,
                   const std::map<stw_types::uint8,
                                  stw_opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds);
   void SigDisconnect(const stw_types::uint32 ou32_NodeIndex,
                      const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber);

private:
   stw_types::uint32 mu32_ViewIndex;
   bool mq_ShowAssignment;
   bool mq_GridSizeSet;
   // All communicating nodes including sub nodes
   // Count of list equals all multi CPU nodes and normal nodes without counting the sub nodes
   stw_types::uint32 mu32_CommunicatingNodeCount;

   stw_types::sint32 m_Init(void);
   stw_types::sint32 m_AppendNode(const stw_types::uint32 ou32_NodeIndex, const bool oq_PartOfSquad);

   void m_ScrollBarRangeChangedVer(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
