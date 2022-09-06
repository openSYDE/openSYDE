//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view widget with current CANopen configuration of Manager and devices on the used interfaces

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOOVERVIEWTABLEVIEW_H
#define C_SDNDECOOVERVIEWTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include "C_TblViewScroll.h"
#include "C_SdNdeCoOverviewTableModel.h"
#include "C_SdNdeCoOverviewDelegate.h"
#include "C_OSCCanInterfaceId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoOverviewTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeCoOverviewTableView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeCoOverviewTableView(void);

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void UpdateData(void);
   void SetNodeIndex(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sintn GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigInterfaceSelected(const stw_types::uint8 ou8_InterfaceNumber);
   void SigDeviceSelected(const stw_types::uint8 ou8_InterfaceNumber,
                          const stw_opensyde_core::C_OSCCanInterfaceId & orc_CanInterfaceId,
                          const stw_types::uint32 ou32_UseCaseIndex);

protected:
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeCoOverviewTableView(const C_SdNdeCoOverviewTableView &);
   C_SdNdeCoOverviewTableView & operator =(const C_SdNdeCoOverviewTableView &);

   void m_InitColumns(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw_opensyde_gui_logic::C_SdNdeCoOverviewTableModel mc_Model;
   stw_opensyde_gui_logic::C_SdNdeCoOverviewDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
