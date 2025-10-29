//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for log jobs list
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBSWIDGET_HPP
#define C_SDNDEDALLOGJOBSWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalLogJobsWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobsWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobsWidget(void) override;

   void InitStaticNames(void) const;
   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void SetNode(const uint32_t ou32_NodeIndex);
   void LoadLogJobs(void);
   void OnLogJobNameModified();
   void LoadSelectedLogJob(const uint32_t ou32_LogJobIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigNumLogJobsChanged(void);
   void SigSelectionChanged(const uint32_t ou32_LogJobIndex);
   void SigShowOverview(const bool oq_Show);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeDalLogJobsWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;
   uint32_t mu32_CurrentLogJobIndex;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   bool mq_IsOverviewVisible;

   void m_OnLogJobStateChanged(const QModelIndex & orc_Index, const bool oq_IsEnabled);
   void m_OnAddLogJob(void);
   void m_OnCutLogJob(void);
   void m_OnCopyLogJob(void);
   void m_OnPasteLogJob(void);
   void m_OnDeleteLogJob(void);
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_UpdateLogJobCountLabel(const uint32_t ou32_LogJobCount);
   void m_ShowOverview(const bool oq_Show);
   void m_OnOverviewClicked(void);
   void m_LogJobSelectionChanged(const uint32_t ou32_LogJobIndex);

   //Avoid call
   C_SdNdeDalLogJobsWidget(const C_SdNdeDalLogJobsWidget &);
   C_SdNdeDalLogJobsWidget & operator =(const C_SdNdeDalLogJobsWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
