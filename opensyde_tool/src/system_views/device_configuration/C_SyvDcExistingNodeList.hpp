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
#include "stwtypes.hpp"
#include "C_SyvDcSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDcExistingNodeList :
   public QListWidget
{
   Q_OBJECT

public:
   C_SyvDcExistingNodeList(QWidget * const opc_Parent = NULL);

   int32_t SetView(const uint32_t ou32_Index, const bool oq_ShowAssignment);
   void ConnectSerialNumber(const uint32_t ou32_NodeIndex,
                            const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber,
                            const std::map<uint8_t,
                                           stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
   const;
   void DisconnectSerialNumber(const uint32_t ou32_NodeIndex,
                               const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber) const;
   uint32_t GetCommunicatingNodeCount(void) const;
   uint32_t GetAssignmentCount(void) const;
   std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceConfiguation> GetConfigs(void) const;

   void StartDrag(const QString & orc_DeviceName, const bool oq_DeviceNameValid) const;
   void StopDrag(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConnect(const uint32_t ou32_NodeIndex,
                   const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber, const std::map<uint8_t,
                                                                                                          stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds);
   void SigDisconnect(const uint32_t ou32_NodeIndex,
                      const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber);

private:
   uint32_t mu32_ViewIndex;
   bool mq_ShowAssignment;
   bool mq_GridSizeSet;
   // All communicating nodes including sub nodes
   // Count of list equals all multi CPU nodes and normal nodes without counting the sub nodes
   uint32_t mu32_CommunicatingNodeCount;

   int32_t m_Init(void);
   int32_t m_AppendNode(const uint32_t ou32_NodeIndex, const bool oq_PartOfSquad);

   void m_ScrollBarRangeChangedVer(const int32_t os32_Min, const int32_t os32_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
