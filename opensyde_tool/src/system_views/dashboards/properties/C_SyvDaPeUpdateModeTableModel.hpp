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
#include "stwtypes.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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

   explicit C_SyvDaPeUpdateModeTableModel(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                          QObject * const opc_Parent = NULL);

   void ApplyData(void) const;

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   static E_Columns h_ColumnToEnum(const int32_t os32_Column);
   static int32_t h_EnumToColumn(const E_Columns oe_Value);

   const stw::opensyde_core::C_OscNodeDataPoolListElementId * GetIndex(const int32_t os32_Index) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigTransmissionCountChange(const int32_t os32_Count);

private:
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_UniqueDataElementIds;
   std::vector<stw::opensyde_gui_logic::C_PuiSvReadDataConfiguration> mc_DataElementConfigurations;
   std::vector<QString> mc_Usage;
   const uint32_t mu32_ViewIndex;

   void m_Init(const uint32_t ou32_NodeIndex);
   static QString mh_TransmissionModeToString(
      const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_TransmissionMode);
   QString m_RailIndexToString(const uint8_t ou8_RailIndex) const;
   static int32_t mh_TransmissionModeToIndex(
      const C_PuiSvReadDataConfiguration::E_TransmissionMode oe_TransmissionMode);
   static C_PuiSvReadDataConfiguration::E_TransmissionMode mh_IndexToTransmissionMode(const int32_t os32_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
