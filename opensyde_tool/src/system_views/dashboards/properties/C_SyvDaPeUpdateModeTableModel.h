//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for dashboard table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODETABLEMODEL_H
#define C_SYVDAPEUPDATEMODETABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_PuiSvReadDataConfiguration.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeUpdateModeTableModel :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   enum E_Columns
   {
      eINDEX = 0,
      eICON,
      eNAME,
      eVALUE_TYPE,
      eARRAY_SIZE,
      eDATA_POOL,
      eTRANSMISSION_MODE,
      eCYCLIC_INTERVAL,
      eTHRESHOLD,
      eUSAGE
   };

   explicit C_SyvDaPeUpdateModeTableModel(const stw_types::uint32 ou32_ViewIndex,
                                          const stw_types::uint32 ou32_NodeIndex, QObject * const opc_Parent = NULL);

   void ApplyData(void) const;

virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const stw_types::sint32 os32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns oe_Value);

   const stw_opensyde_core::C_OSCNodeDataPoolListElementId * GetIndex(const stw_types::sint32 os32_Index) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigTransmissionCountChange(const stw_types::sint32 os32_Count);

private:
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_UniqueDataElementIds;
   std::vector<stw_opensyde_gui_logic::C_PuiSvReadDataConfiguration> mc_DataElementConfigurations;
   std::vector<QString> mc_Usage;
   const stw_types::uint32 mu32_ViewIndex;

   void m_Init(const stw_types::uint32 ou32_NodeIndex);
   static QString mh_TransmissionModeToString(
      const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_TransmissionMode);
   QString m_RailIndexToString(const stw_types::uint8 ou8_RailIndex) const;
   static stw_types::sintn mh_TransmissionModeToIndex(
      const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_TransmissionMode);
   static C_PuiSvReadDataConfiguration::E_TransmissionMode mh_IndexToTransmissionMode(const stw_types::sintn osn_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
