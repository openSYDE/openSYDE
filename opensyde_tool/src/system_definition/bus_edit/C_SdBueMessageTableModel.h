//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message table model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGETABLEMODEL_H
#define C_SDBUEMESSAGETABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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

   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);
   stw_types::sint32 ConvertRowToMessage(const stw_types::sint32 & ors32_Row,
                                         stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;

private:
   struct C_MsgTableData
   {
      QString c_Name;
      QString c_Comment;
      QVariant c_CoIndex;
      QVariant c_CoIndexEditRole;
      stw_types::sintn sn_Enabled;
      stw_types::sintn sn_Extended;
      QVariant c_CobId;
      QVariant c_CobIdEditRole;
      QString c_CanId;
      QString c_CanIdEditRole;
      stw_types::uint16 u16_Dlc;
      QString c_TxMethod;
      QString c_CycleTime;
      QVariant c_NotEarlierThan;
      QVariant c_NotLaterThan;
      QString c_Transmitter;
      QString c_Receiver;
      QVariant c_Icon;
   };

   struct C_MsgTableConfig
   {
      stw_opensyde_core::C_OSCCanMessageIdentificationIndices c_MessageId;
      C_MsgTableData c_MessageData;
   };

   std::vector<C_MsgTableConfig> mc_MsgInfoAll;

   C_PuiSdNodeCanMessageSyncManager * mpc_SyncManager;

   QString m_CreateNodeName(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_CurMatchingId,
                            const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_AllMatchingIds)
   const;
   QString m_GetNotEarlierThanHeaderName(void) const;
   QString m_GetNotLaterThanHeaderName(void) const;
   QVariant m_GetCanOpenIndex(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Id,
                              const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                              const stw_types::sintn osn_Role) const;
   QVariant m_GetCobId(const stw_opensyde_core::C_OSCCanMessage & orc_Message, const stw_types::sintn osn_Role) const;
   QVariant m_GetNotEarlierThanValue(const stw_opensyde_core::C_OSCCanMessage & orc_Message) const;
   QVariant m_GetNotLaterThanValue(const stw_opensyde_core::C_OSCCanMessage & orc_Message) const;
   QString m_GetTransmitter(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Id,
                            const stw_opensyde_core::C_OSCCanMessage & orc_Message) const;
   QString m_GetReceiver(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Id,
                         const stw_opensyde_core::C_OSCCanMessage & orc_Message) const;
   QString m_GetCanOpenNode(const stw_opensyde_core::C_OSCCanMessage & orc_Message) const;
   QVariant m_GetMessageIcon(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;

   void m_FillMsgInfo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
