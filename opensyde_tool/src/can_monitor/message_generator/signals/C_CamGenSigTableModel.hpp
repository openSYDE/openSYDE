//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for message generator signals table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENSIGTABLEMODEL_H
#define C_CAMGENSIGTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_CieConverter.hpp"
#include "C_OscCanMessage.hpp"
#include "C_CamProMessageData.hpp"
#include "C_OscNodeDataPoolList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenSigTableModel :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   enum E_Columns
   {
      eBIT_POS = 0,
      eNAME,
      eRAW,
      ePHYSICAL,
      eUNIT
   };

   C_CamGenSigTableModel(QObject * const opc_Parent = NULL);

   void TriggerSignalReload(void);
   void SetMessage(const uint32_t ou32_Message);
   void UpdateMessageDlc(const uint32_t ou32_MessageIndex);

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const int32_t os32_Column);
   static int32_t h_EnumToColumn(const E_Columns oe_Value);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigResetPermanentEditors(void);
   void SigUpdateMessageData(const uint32_t ou32_MessageIndex);
   void SigTriggerModelUpdateCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);

private:
   uint32_t mu32_MessageIndex;

   bool m_CheckInterpretedMode(void) const;
   const stw::opensyde_core::C_OscCanMessage * m_GetMessageInterpretedOsy(
      stw::opensyde_core::C_OscCanProtocol::E_Type * const ope_ProtocolType = NULL) const;
   const stw::opensyde_core::C_OscCanSignal * m_GetSignalInterpretedOsy(const uint32_t ou32_Index,
                                                                        stw::opensyde_core::C_OscCanProtocol::E_Type * const ope_ProtocolType = NULL)
   const;
   const stw::opensyde_core::C_OscNodeDataPoolList * m_GetMessageListInterpreted(void) const;
   const stw::opensyde_core::C_OscNodeDataPoolListElement * m_GetSignalInterpretedOsyCommon(const uint32_t ou32_Index)
   const;
   const C_CieConverter::C_CieCanMessage * m_GetMessageInterpretedDbc(void) const;
   const C_CieConverter::C_CieCanSignal * m_GetSignalInterpretedDbc(const uint32_t ou32_Index) const;
   static stw::opensyde_core::C_OscNodeDataPoolContent mh_DecodeRawToContentDbc(const std::vector<uint8_t> & orc_Raw,
                                                                                const C_CieConverter::C_CieCanSignal & orc_Signal);
   bool m_CheckUseCheckBox(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                           const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max) const;
   QVariant m_HandleColRawInterpreted(const uint32_t ou32_Index, const int32_t os32_Role) const;
   QVariant m_HandleColPhysicalInterpreted(const uint32_t ou32_Index, const int32_t os32_Role) const;
   static stw::opensyde_core::C_OscNodeDataPoolContent mh_GetInitialValue(
      const stw::opensyde_core::C_OscNodeDataPoolListElement * const opc_OsySignalCommon,
      const C_CieConverter::C_CieCanSignal * const opc_DbcSignal);
   static void mh_SetBoolInContent(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value, const bool oq_Value);
   int32_t m_SetSignalFromOsyValue(const stw::opensyde_core::C_OscCanSignal & orc_OsySignal,
                                   const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value);
   static stw::opensyde_core::C_OscNodeDataPoolContent mh_GetBorderValue(
      const stw::opensyde_core::C_OscNodeDataPoolContent & orc_InitValue, const uint16_t ou16_BitLength,
      const bool oq_IsMin);
   static uint64_t mh_GetMax(const uint16_t ou16_Bit);
   static bool mh_IsMultiplexed(const stw::opensyde_core::C_OscCanMessage * const opc_OsyMessage,
                                const C_CieConverter::C_CieCanMessage * const opc_DbcMessage);
   uint16_t m_GetMultiplexerValue(const stw::opensyde_core::C_OscCanMessage * const opc_OsyMessage,
                                  const C_CieConverter::C_CieCanMessage * const opc_DbcMessage) const;
   static int32_t mh_GetNumRowsForMuxValue(const stw::opensyde_core::C_OscCanMessage * const opc_OsyMessage,
                                           const C_CieConverter::C_CieCanMessage * const opc_DbcMessage,
                                           const uint16_t ou16_MuxValue);
   uint32_t m_TranslateRowToIndex(const int32_t os32_Row) const;
   static std::vector<uint16_t> mh_GetStartBits(const stw::opensyde_core::C_OscCanMessage * const opc_OsyMessage,
                                                const C_CieConverter::C_CieCanMessage * const opc_DbcMessage,
                                                const uint16_t ou16_MuxValue);
   void m_ResetUnusedMultiplexedSignals(void);
   void m_UpdateMultiplexedSignal(const stw::opensyde_core::C_OscCanMessage * const opc_OsyMessage,
                                  const C_CieConverter::C_CieCanMessage * const opc_DbcMessage,
                                  const uint16_t ou16_MuxValue, const uint32_t ou32_SignalIndex,
                                  const bool oq_SetToZero);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
