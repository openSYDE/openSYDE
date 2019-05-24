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
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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

   void SetIndex(const stw_types::uint32 ou32_NodeIndex);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   // Header:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   // Editable:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;

   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);

private:
   static const stw_types::sintn mhsn_INDEX_STANDARD;
   static const stw_types::sintn mhsn_INDEX_EXTENDED;
   stw_types::uint32 mu32_NodeIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
