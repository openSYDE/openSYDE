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
      eLOGJOB_ADDED,
      eSELECTINDEX
   };

   C_SdNdeDalLogJobsListView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobsListView(void) override;

   bool IsEmpty(void) const;
   void LoadLogJobs(const uint32_t ou32_NodeIndex);
   void SetSelection(const E_LogJobSelection & ore_Selection, const uint32_t ou32_LogJobIndex = 0);
   void LoadSelectedLogJob(const uint32_t ou32_LogJobIndex);
   void DeleteLogJobs(const std::vector<uint32_t> & orc_DataLoggerJobIndices);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDataChanged(const QModelIndex & orc_Index, const bool oq_IsEnabled);
   void SigSelectionChanged(const uint32_t ou32_LogJobIndex);

protected:
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   void m_LogJobDeleted(const QModelIndex & orc_Parent, const int32_t os32_First, const int32_t os32_Last);
   void m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
   void m_ShowHideHorizontalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;

   int32_t ms32_DeletedItemIndex;
   bool mq_SelectAllItems;
   stw::opensyde_gui_logic::C_SdNdeDalLogJobsListModel mc_Model;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
