//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Log Jobs overview table model
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBSOVTABLEMODEL_H
#define C_SDNDEDALLOGJOBSOVTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobsOvTableModel :
   public QAbstractTableModel
{
public:
   C_SdNdeDalLogJobsOvTableModel(QObject * const opc_Parent = NULL);

   enum E_Columns
   {
      eINDEX = 0,
      eICON,
      eNAME,
      eCOMMENT,
      eUSE_CASE,
      eTRIGGERMODE,
      eLOGGINGDATA
   };

   void SetNodeIndex(const uint32_t ou32_NodeIndex);
   void UpdateData(const std::vector<std::tuple<uint32_t, uint32_t,
                                                uint32_t> > & orc_AllLoggerJobElementsLocationCount);
   uint32_t GetNodeIndex(void) const;

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation, const int32_t os32_Role =
                          static_cast<int32_t>(Qt::DisplayRole)) const override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   static E_Columns h_ColumnToEnum(const int32_t & ors32_Column);
   static int32_t h_EnumToColumn(const E_Columns & ore_Value);

private:
   class C_LogJobOvTableData
   {
   public:
      QString c_Index;
      QString c_LogJobName;
      QString c_LogJobComment;
      QString c_LogJobUseCase;
      QString c_LogJobTriggerMode;
      QString c_LogJobLoggingData;
      QVariant c_LogJobIcon;
   };

   void m_FillLogJobsInfo(const std::vector<std::tuple<uint32_t, uint32_t,
                                                       uint32_t> > & orc_AllLoggerJobElementsLocationCount);

   uint32_t mu32_NodeIndex;
   std::vector<C_LogJobOvTableData> mc_LogJobsInfoAll;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
