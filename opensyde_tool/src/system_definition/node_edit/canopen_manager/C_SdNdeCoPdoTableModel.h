//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for showing the PDO configuration of a specific device on a specific interface of
                the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOPDOTABLEMODEL_H
#define C_SDNDECOPDOTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include <vector>
#include <stwtypes.h>
#include "C_OSCCanInterfaceId.h"
#include "C_OSCCanMessage.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoPdoTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eINDEX = 0,
      eENABLED,
      eNAME,
      eCOMMENT,
      eCOTYPE,
      eCOBID,
      eEXTENDED,
      eDLC,
      eTXMETHOD,
      eINHIBITTIME,
      eEVENTTIME
   };

   C_SdNdeCoPdoTableModel(QObject * const opc_Parent = NULL);

   void UpdateData(void);
   stw_types::uint32 GetDeviceNodeIndex(void) const;
   void GetNodeIndexAndInterfaceId(stw_types::uint32 & oru32_NodeIndex, stw_types::uint8 & oru8_InterfaceId,
                                   stw_opensyde_core::C_OSCCanInterfaceId & orc_CanNodeInterfaceId);
   void SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_ManagerNodeIndex,
                                   const stw_types::uint8 ou8_ManagerInterfaceId,
                                   const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceNodeInterfaceId);
   stw_types::sint32 MapRowToMsgId(const stw_types::sint32 os32_Index,
                                   stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MsgId) const;

   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

private:
   struct C_PdoTableData
   {
      bool q_Enabled;
      QString c_Name;
      QString c_Comment;
      QString c_CoType;
      bool q_Extended;
      QString c_CobId;
      QString c_Dlc;
      QString c_TxMethod;
      QString c_InhibitTime;
      QString c_EventTime;
      stw_opensyde_core::C_OSCCanMessageIdentificationIndices c_MsgId;
   };

   enum E_PdoType
   {
      eTX,
      eRX
   };

   C_PdoTableData m_GetPdoMessageInfo(const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessagegId,
                                      const E_PdoType oe_PdoType) const;
   void m_FillPdoInfo(void);

   stw_types::uint32 mu32_ManagerNodeIndex;
   stw_types::uint8 mu8_ManagerInterfaceId;
   stw_opensyde_core::C_OSCCanInterfaceId mc_DeviceNodeInterfaceId;
   std::vector<C_PdoTableData> mc_PdoTableData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
