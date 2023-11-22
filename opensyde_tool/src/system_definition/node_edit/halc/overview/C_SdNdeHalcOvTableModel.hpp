//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview table model
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVTABLEMODEL_HPP
#define C_SDNDEHALCOVTABLEMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include "stwtypes.hpp"
#include "C_OscHalcConfigDomain.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcOvTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eINDEX = 0,
      eDOMAIN,
      eCHANNEL,
      eNAME,
      eCOMMENT,
      eUSE_CASE,
      eLINKED_WITH,
      eSAFETY_RELEVANT,
      eTYPE,
      eDATAPOOL,
      eDATABLOCK_ASSIGNMENT
   };

   C_SdNdeHalcOvTableModel(QObject * const opc_Parent = NULL);

   void UpdateData(void);
   uint32_t GetNodeIndex(void) const;
   void SetNodeIndex(const uint32_t ou32_NodeIndex);

   int32_t MapRowToDomainIndex(const int32_t os32_Index, uint32_t & oru32_DomainIndex) const;
   int32_t MapRowToChannelNum(const int32_t os32_Index, uint32_t & oru32_ChannelNum) const;

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

private:
   class C_HalcTableData
   {
   public:
      QString c_DomainName;
      QString c_ChannelName;
      QString c_CustomName;
      QString c_Comment;
      QString c_UseCase;
      QVariant c_LinkedWith;
      QString c_Type;
      QString c_DatapoolName;
      QString c_DatablockAssignment;
      int32_t s32_SafetyRelevant;
      QVariant c_Icon;
   };

   class C_HalcTableConfig
   {
   public:
      bool q_DomainOnly;
      uint32_t u32_DomainIndex;
      uint32_t u32_ChannelIndex;
      C_HalcTableData c_Data;
   };

   uint32_t mu32_NodeIndex;
   std::vector<C_HalcTableConfig> mc_HalcInfoAll;

   void m_FillHalcInfo(void);
   void m_FillHalcInfoLinked(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                             const bool oq_UseChannelIndex, C_HalcTableData & orc_Data);
   void m_FillHalcInfoIcon(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                           const bool oq_UseChannelIndex,
                           const stw::opensyde_core::C_OscHalcDefDomain::E_Category oe_Category,
                           C_HalcTableData & orc_Data);
   void m_FillHalcInfoData(const stw::opensyde_core::C_OscHalcConfigDomain & orc_Domain,
                           const stw::opensyde_core::C_OscHalcConfigChannel & orc_Config, C_HalcTableData & orc_Data);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
