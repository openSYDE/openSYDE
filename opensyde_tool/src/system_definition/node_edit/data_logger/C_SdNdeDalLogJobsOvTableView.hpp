//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Log Jobs overview table view
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBSOVTABLEVIEW_H
#define C_SDNDEDALLOGJOBSOVTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include <QScrollBar>
#include "C_TblViewScroll.hpp"
#include "C_SdNdeDalLogJobsOvTableModel.hpp"
#include "C_SdNdeDalLogJobOvDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobsOvTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDalLogJobsOvTableView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobsOvTableView(void) override;
   void UpdateData(const std::vector<std::tuple<uint32_t, uint32_t,
                                                uint32_t> > & orc_AllLoggerJobElementsLocationCount);
   int32_t GetCountRows(void) const;
   void SetNodeIndex(const uint32_t ou32_NodeIndex);
   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigLoadSelectedLogJob(uint32_t ou32_LogJobIndex);

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   QSortFilterProxyModel mc_SortProxyModel;
   stw::opensyde_gui_logic::C_SdNdeDalLogJobsOvTableModel mc_Model;
   stw::opensyde_gui_logic::C_SdNdeDalLogJobOvDelegate mc_Delegate;

   void m_InitColumns(void);
   void m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
   void m_ShowHideHorizontalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;

   //Avoid call
   C_SdNdeDalLogJobsOvTableView(const C_SdNdeDalLogJobsOvTableView &);
   C_SdNdeDalLogJobsOvTableView & operator =(const C_SdNdeDalLogJobsOvTableView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
