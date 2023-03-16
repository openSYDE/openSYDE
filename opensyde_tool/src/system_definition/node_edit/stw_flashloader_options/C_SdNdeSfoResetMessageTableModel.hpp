//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDESFORESETMESSAGETABLEMODEL_H
#define C_SDNDESFORESETMESSAGETABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeSfoResetMessageTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eID = 0,
      eEXTENDED_FORMAT,
      eDLC,
      eBYTE1,
      eBYTE2,
      eBYTE3,
      eBYTE4,
      eBYTE5,
      eBYTE6,
      eBYTE7,
      eBYTE8
   };

   explicit C_SdNdeSfoResetMessageTableModel(QObject * const opc_Parent = NULL);

   void SetIndex(const uint32_t ou32_NodeIndex);

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;

   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

private:
   static const int32_t mhs32_INDEX_STANDARD;
   static const int32_t mhs32_INDEX_EXTENDED;
   uint32_t mu32_NodeIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
