//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model for log job data selection

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBDATASELECTIONTABLEMODEL_H
#define C_SDNDEDALLOGJOBDATASELECTIONTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAbstractTableModel>

#include "C_OscDataLoggerDataElementReference.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobDataSelectionTableModel :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   enum E_Columns
   {
      eDATA_ELEMENT = 0,
      eLOCATION,
      eNAMESPACE,
      eLOGGING_NAME,
      eCOMMENT
   };

   explicit C_SdNdeDalLogJobDataSelectionTableModel(QObject * const opc_Parent = NULL);
   void UpdateData(const std::vector<stw::opensyde_core::C_OscDataLoggerDataElementReference> & orc_DataElements,
                   const uint32_t ou32_NodeIndex);

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;

   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDataChanged(const QModelIndex orc_Index, const QString oc_Data);

private:
   class C_DalLogJobData
   {
   public:
      //      uint32_t u32_DataElementId;
      stw::scl::C_SclString c_DataElementName;
      stw::scl::C_SclString c_Location;
      stw::scl::C_SclString c_Namespace;
      stw::scl::C_SclString c_LoggingName;
      stw::scl::C_SclString c_Comment;
   };

   std::vector<C_DalLogJobData> mc_LoggingDataList;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
