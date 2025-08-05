//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List view for log jobs
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBSLISTVIEW_H
#define C_SDNDEDALLOGJOBSLISTVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QListView>

#include "C_SdNdeDalLogJobsListModel.hpp"
#include  "C_SclStringList.hpp"
#include "C_OscDataLoggerJob.hpp"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobsListView :
   public QListView
{
   Q_OBJECT

public:
   enum E_LogJobSelection
   {
      eINITIALIZE = 0,
      eLOGJOB_ADDED
   };

   C_SdNdeDalLogJobsListView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobsListView(void) override;

   bool IsEmpty(void) const;
   void LoadLogJobs(const uint32_t ou32_NodeIndex);
   void SetSelection(const E_LogJobSelection & ore_Selection);
   void DeleteLogJob(const uint32_t ou32_LogJobIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDataChanged(const QModelIndex orc_Index, const bool oq_IsEnabled);
   void SigSelectionChanged(const uint32_t ou32_LogJobIndex);
   void SigNumLogJobsChanged();

protected:
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;

private:
   int32_t ms32_DeletedItemIndex;
   stw::opensyde_gui_logic::C_SdNdeDalLogJobsListModel mc_Model;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
