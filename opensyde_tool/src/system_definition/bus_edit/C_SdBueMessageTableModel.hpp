//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message table model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGETABLEMODEL_HPP
#define C_SDBUEMESSAGETABLEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMessageTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eINDEX = 0,
      eICON,
      eENABLED,
      eNAME,
      eCOMMENT,
      eJ1939_PGN,
      eJ1939_PRIORITY,
      eJ1939_SOURCE_ADDRESS,
      eJ1939_DESTINATION_ADDRESS,
      eJ1939_FORMAT,
      eJ1939_EDP_AND_DP,
      eCAN_OPEN_INDEX,
      eEXTENDED,
      eCAN_ID,
      eCOB_ID,
      eDLC,
      eTX_METHOD,
      eCYCLE_TIME,
      eNOT_EARLIER_THAN,
      eNOT_LATER_THAN,
      eTRANSMITTER,
      eRECEIVER
   };

   explicit C_SdBueMessageTableModel(QObject * const opc_Parent = NULL);
   void SetMessageSyncManager(C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void UpdateData(void);

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);
   int32_t ConvertRowToMessage(const int32_t & ors32_Row,
                               stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;

private:
   class C_MsgTableData
   {
   public:
      QString c_Name;
      QString c_Comment;
      QVariant c_J1939Pgn;
      QVariant c_J1939Priority;
      QVariant c_J1939SourceAddress;
      QVariant c_J1939DestinationAddress;
      QVariant c_J1939Format;
      QVariant c_J1939EdpAndDp;
      QVariant c_CoIndex;
      QVariant c_CoIndexEditRole;
      int32_t s32_Enabled;
      int32_t s32_Extended;
      QVariant c_CobId;
      QVariant c_CobIdEditRole;
      QString c_CanId;
      QString c_CanIdEditRole;
      uint16_t u16_Dlc;
      QString c_TxMethod;
      QString c_CycleTime;
      QVariant c_NotEarlierThan;
      QVariant c_NotLaterThan;
      QString c_Transmitter;
      QString c_Receiver;
      QVariant c_Icon;
   };

   class C_MsgTableConfig
   {
   public:
      stw::opensyde_core::C_OscCanMessageIdentificationIndices c_MessageId;
      C_MsgTableData c_MessageData;
   };

   std::vector<C_MsgTableConfig> mc_MsgInfoAll;

   C_PuiSdNodeCanMessageSyncManager * mpc_SyncManager;

   QString m_CreateNodeName(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_CurMatchingId,
                            const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_AllMatchingIds)
   const;
   QString m_GetNotEarlierThanHeaderName(void) const;
   QString m_GetNotLaterThanHeaderName(void) const;
   QVariant m_GetCanOpenIndex(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_Id,
                              const stw::opensyde_core::C_OscCanMessage & orc_Message, const int32_t os32_Role) const;
   QVariant m_GetCobId(const stw::opensyde_core::C_OscCanMessage & orc_Message, const int32_t os32_Role) const;
   QVariant m_GetNotEarlierThanValue(const stw::opensyde_core::C_OscCanMessage & orc_Message) const;
   QVariant m_GetNotLaterThanValue(const stw::opensyde_core::C_OscCanMessage & orc_Message) const;
   QString m_GetTransmitter(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_Id,
                            const stw::opensyde_core::C_OscCanMessage & orc_Message) const;
   QString m_GetReceiver(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_Id,
                         const stw::opensyde_core::C_OscCanMessage & orc_Message) const;
   QString m_GetCanOpenNode(const stw::opensyde_core::C_OscCanMessage & orc_Message) const;
   QVariant m_GetMessageIcon(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;

   void m_FillMsgInfo(void);
   void m_FillJ1939MsgInfo(C_MsgTableData & orc_MessageTableData,
                           const stw::opensyde_core::C_OscCanMessage & orc_MessageData);
   QString m_GetCycleTimeHeaderName(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
