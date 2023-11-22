//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for showing the PDO configuration of a specific device on a specific interface of
                the CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOPDOTABLEMODEL_HPP
#define C_SDNDECOPDOTABLEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include <vector>
#include <stwtypes.h>
#include "C_OscCanInterfaceId.hpp"
#include "C_OscCanMessage.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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
   uint32_t GetDeviceNodeIndex(void) const;
   void GetNodeIndexAndInterfaceId(uint32_t & oru32_NodeIndex, uint8_t & oru8_InterfaceId,
                                   stw::opensyde_core::C_OscCanInterfaceId & orc_CanNodeInterfaceId);
   void SetNodeIndexAndInterfaceId(const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerInterfaceId,
                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeInterfaceId);
   int32_t MapRowToMsgId(const int32_t os32_Index,
                         stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MsgId) const;

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;

   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

private:
   class C_PdoTableData
   {
   public:
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
      stw::opensyde_core::C_OscCanMessageIdentificationIndices c_MsgId;
   };

   enum E_PdoType
   {
      eTX,
      eRX
   };

   C_PdoTableData m_GetPdoMessageInfo(const stw::opensyde_core::C_OscCanMessage & orc_Message,
                                      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessagegId,
                                      const E_PdoType oe_PdoType) const;
   void m_FillPdoInfo(void);

   uint32_t mu32_ManagerNodeIndex;
   uint8_t mu8_ManagerInterfaceId;
   stw::opensyde_core::C_OscCanInterfaceId mc_DeviceNodeInterfaceId;
   std::vector<C_PdoTableData> mc_PdoTableData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
