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
#include "stwtypes.h"
#include "C_CieConverter.h"
#include "C_OSCCanMessage.h"
#include "C_CamProMessageData.h"
#include "C_OSCNodeDataPoolList.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
   void SetMessage(const stw_types::uint32 ou32_Message);
   void UpdateMessageDLC(const stw_types::uint32 ou32_MessageIndex);

   // Basic functionality:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;

   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

   static E_Columns h_ColumnToEnum(const stw_types::sint32 os32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns oe_Value);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigResetPermanentEditors(void);
   void SigUpdateMessageData(const stw_types::uint32 ou32_MessageIndex);
   void SigTriggerModelUpdateCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);

private:
   stw_types::uint32 mu32_MessageIndex;

   bool m_CheckInterpretedMode(void) const;
   const stw_opensyde_core::C_OSCCanMessage * m_GetMessageInterpretedOsy(void) const;
   const stw_opensyde_core::C_OSCCanSignal * m_GetSignalInterpretedOsy(const stw_types::uint32 ou32_Index) const;
   const stw_opensyde_core::C_OSCNodeDataPoolList * m_GetMessageListInterpreted(void) const;
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * m_GetSignalInterpretedOsyCommon(
      const stw_types::uint32 ou32_Index) const;
   const C_CieConverter::C_CIECanMessage * m_GetMessageInterpretedDbc(void) const;
   const C_CieConverter::C_CIECanSignal * m_GetSignalInterpretedDbc(const stw_types::uint32 ou32_Index) const;
   static stw_opensyde_core::C_OSCNodeDataPoolContent mh_DecodeRawToContentDbc(
      const std::vector<stw_types::uint8> & orc_Raw, const C_CieConverter::C_CIECanSignal & orc_Signal);
   bool m_CheckUseCheckBox(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                           const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max) const;
   QVariant m_HandleColRawInterpreted(const stw_types::uint32 ou32_Index, const stw_types::sintn osn_Role) const;
   QVariant m_HandleColPhysicalInterpreted(const stw_types::uint32 ou32_Index, const stw_types::sintn osn_Role) const;
   static void mh_SetBoolInContent(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Value, const bool oq_Value);
   static stw_opensyde_core::C_OSCNodeDataPoolContent mh_GetBorderValue(
      const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_InitValue, const stw_types::uint16 ou16_BitLength,
      const bool oq_IsMin);
   static stw_types::uint64 mh_GetMax(const stw_types::uint16 ou16_Bit);
   static bool mh_IsMultiplexed(const stw_opensyde_core::C_OSCCanMessage * const opc_OsyMessage,
                                const C_CieConverter::C_CIECanMessage * const opc_DbcMessage);
   stw_types::uint16 m_GetMultiplexerValue(const stw_opensyde_core::C_OSCCanMessage * const opc_OsyMessage,
                                           const C_CieConverter::C_CIECanMessage * const opc_DbcMessage) const;
   static stw_types::sint32 mh_GetNumRowsForMuxValue(const stw_opensyde_core::C_OSCCanMessage * const opc_OsyMessage,
                                                     const C_CieConverter::C_CIECanMessage * const opc_DbcMessage,
                                                     const stw_types::uint16 ou16_MuxValue);
   stw_types::uint32 m_TranslateRowToIndex(const stw_types::sint32 os32_Row) const;
   static std::vector<stw_types::uint16> mh_GetStartBits(
      const stw_opensyde_core::C_OSCCanMessage * const opc_OsyMessage,
      const C_CieConverter::C_CIECanMessage * const opc_DbcMessage, const stw_types::uint16 ou16_MuxValue);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
