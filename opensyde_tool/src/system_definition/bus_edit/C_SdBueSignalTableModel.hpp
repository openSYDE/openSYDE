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

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);
   int32_t ConvertRowToSignal(const int32_t & ors32_Row,
                              stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                              uint32_t & oru32_Signal) const;

private:
   class C_SigTableData
   {
   public:
      QString c_MsgName;
      QString c_CoIndex;
      QString c_MultiplexingValue;
      QString c_SigName;
      QString c_Comment;
      uint16_t u16_StartBit;
      uint16_t u16_BitLength;
      QString c_ByteOrder;
      QString c_ValueType;
      QVariant c_InitValue;
      QVariant c_Factor;
      QVariant c_Offset;
      int32_t s32_AutoMinMax;
      QVariant c_MinValue;
      QVariant c_MaxValue;
      QString c_Unit;
      QVariant c_Icon;
   };

   class C_SigTableConfig
   {
   public:
      stw::opensyde_core::C_OscCanMessageIdentificationIndices c_MessageId;
      uint32_t u32_SignalIndex;
      C_SigTableData c_SignalData;
   };

   std::vector<C_SigTableConfig> mc_SigInfoAll;

   C_PuiSdNodeCanMessageSyncManager * mpc_SyncManager;

   QString m_GetMultiplexingValue(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                  const uint32_t ou32_Signal) const;
   QVariant m_GetDisplayAndEditValue(const QModelIndex & orc_Index, const E_Columns oe_Column) const;
   QFont m_GetFontValue(const E_Columns oe_Column) const;

   void m_FillSigInfo(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
