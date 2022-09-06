//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal table model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUESIGNALTABLEMODEL_H
#define C_SDBUESIGNALTABLEMODEL_H

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

class C_SdBueSignalTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eINDEX = 0,
      eICON,
      eMESSAGE,
      eNAME,
      eCOMMENT,
      eCAN_OPEN_INDEX,
      eMULTIPLEXING,
      eSTART_BIT,
      eLENGTH,
      eBYTE_ORDER,
      eVALUE_TYPE,
      eINITIAL_VALUE,
      eFACTOR,
      eOFFSET,
      eAUTO_MIN_MAX,
      eMINIMUM_VALUE,
      eMAXIMUM_VALUE,
      eUNIT
   };

   explicit C_SdBueSignalTableModel(QObject * const opc_Parent = NULL);
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
   stw_types::sint32 ConvertRowToSignal(const stw_types::sint32 & ors32_Row,
                                        stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        stw_types::uint32 & oru32_Signal) const;

private:
   struct C_SigTableData
   {
      QString c_MsgName;
      QString c_CoIndex;
      QString c_MultiplexingValue;
      QString c_SigName;
      QString c_Comment;
      stw_types::uint16 u16_StartBit;
      stw_types::uint16 u16_BitLength;
      QString c_ByteOrder;
      QString c_ValueType;
      QVariant c_InitValue;
      QVariant c_Factor;
      QVariant c_Offset;
      stw_types::sintn sn_AutoMinMax;
      QVariant c_MinValue;
      QVariant c_MaxValue;
      QString c_Unit;
      QVariant c_Icon;
   };

   struct C_SigTableConfig
   {
      stw_opensyde_core::C_OSCCanMessageIdentificationIndices c_MessageId;
      stw_types::uint32 u32_SignalIndex;
      C_SigTableData c_SignalData;
   };

   std::vector<C_SigTableConfig> mc_SigInfoAll;

   C_PuiSdNodeCanMessageSyncManager * mpc_SyncManager;

   QString m_GetMultiplexingValue(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                  const stw_types::uint32 ou32_Signal) const;
   QVariant m_GetDisplayAndEditValue(const QModelIndex & orc_Index, const E_Columns oe_Column) const;
   QFont m_GetFontValue(const E_Columns oe_Column) const;

   void m_FillSigInfo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
