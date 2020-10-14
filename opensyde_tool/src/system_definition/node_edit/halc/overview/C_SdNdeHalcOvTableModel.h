//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview table model
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVTABLEMODEL_H
#define C_SDNDEHALCOVTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_OSCHalcConfigDomain.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
   stw_types::uint32 GetNodeIndex(void) const;
   void SetNodeIndex(const stw_types::uint32 ou32_NodeIndex);

   const stw_opensyde_core::C_OSCHalcConfigChannel * MapRowToChannel(const stw_types::sint32 os32_Index) const;
   stw_types::sint32 MapRowToDomainIndex(const stw_types::sint32 os32_Index,
                                         stw_types::uint32 & oru32_DomainIndex) const;
   stw_types::sint32 MapRowToChannelNum(const stw_types::sint32 os32_Index, stw_types::uint32 & oru32_ChannelNum) const;
   const stw_opensyde_core::C_OSCHalcDefChannelDef * MapRowToChannelDef(const stw_types::sint32 os32_Index) const;
   const stw_opensyde_core::C_OSCHalcConfigDomain * MapRowToDomain(const stw_types::sint32 os32_Index) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);

private:
   stw_types::uint32 mu32_NodeIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
