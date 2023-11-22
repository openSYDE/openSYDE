//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview table view
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVTABLEVIEW_HPP
#define C_SDNDEHALCOVTABLEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include "stwtypes.hpp"
#include "C_TblViewScroll.hpp"
#include "C_SdNdeHalcOvTableModel.hpp"
#include "C_SdNdeHalcOvDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcOvTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeHalcOvTableView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcOvTableView(void) override;

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void UpdateData(void);
   void SetNodeIndex(const uint32_t ou32_NodeIndex);
   int32_t GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChannelSelected(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                           const bool oq_UseChannelIndex);

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeHalcOvTableView(const C_SdNdeHalcOvTableView &);
   C_SdNdeHalcOvTableView & operator =(const C_SdNdeHalcOvTableView &) &;

   void m_InitColumns(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw::opensyde_gui_logic::C_SdNdeHalcOvTableModel mc_Model;
   stw::opensyde_gui_logic::C_SdNdeHalcOvDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
