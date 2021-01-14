//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview table view
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVTABLEVIEW_H
#define C_SDNDEHALCOVTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include "stwtypes.h"
#include "C_TblViewScroll.h"
#include "C_SdNdeHalcOvTableModel.h"
#include "C_SdNdeHalcOvDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcOvTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeHalcOvTableView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeHalcOvTableView(void);

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void UpdateData(void);
   void SetNodeIndex(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sintn GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChannelSelected(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                           const bool oq_UseChannelIndex);

protected:
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeHalcOvTableView(const C_SdNdeHalcOvTableView &);
   C_SdNdeHalcOvTableView & operator =(const C_SdNdeHalcOvTableView &);

   void m_InitColumns(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw_opensyde_gui_logic::C_SdNdeHalcOvTableModel mc_Model;
   stw_opensyde_gui_logic::C_SdNdeHalcOvDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
