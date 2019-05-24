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
      eNAME,
      eCOMMENT,
      eEXTENDED,
      eCAN_ID,
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
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);
   stw_types::sint32 ConvertRowToMessage(const stw_types::sint32 & ors32_Row,
                                         stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;

private:
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mc_MessageIds;
   C_PuiSdNodeCanMessageSyncManager * mpc_SyncManager;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
