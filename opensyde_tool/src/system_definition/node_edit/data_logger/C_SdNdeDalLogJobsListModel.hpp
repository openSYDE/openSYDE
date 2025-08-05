//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List model for log jobs
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBSLISTMODEL_H
#define C_SDNDEDALLOGJOBSLISTMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QAbstractListModel>

#include "C_SclStringList.hpp"
#include "C_OscDataLoggerJob.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------------------------------------------------
   */

/* -- Types ---------------------------------------------------------------------------------------------------------
   */

class C_SdNdeDalLogJobsListModel :
   public QAbstractListModel
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobsListModel(QObject * const opc_Parent = NULL);
   void UpdateData(const uint32_t ou32_NodeIndex);
   void DoRemoveRow(const uint32_t ou32_ElementIndex);

   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   Qt::ItemFlags flags (const QModelIndex & orc_Index) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDataChanged(const QModelIndex orc_Index, const bool oq_IsEnabled);

private:
   class C_DalLogJob
   {
   public:
      stw::scl::C_SclString c_Name;
      bool q_Enabled;
   };

   std::vector<C_DalLogJob> mc_LogJobsList;
};

/* -- Extern Global Variables ---------------------------------------------------------------------------------------
   */
}
} //end of namespace

#endif
