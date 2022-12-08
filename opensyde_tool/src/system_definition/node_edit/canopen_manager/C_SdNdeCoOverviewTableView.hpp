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
#include "C_TblViewScroll.hpp"
#include "C_SdNdeCoOverviewTableModel.hpp"
#include "C_SdNdeCoOverviewDelegate.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
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
   void SetNodeIndex(const uint32_t ou32_NodeIndex);
   int32_t GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigInterfaceSelected(const uint8_t ou8_InterfaceNumber);
   void SigDeviceSelected(const uint8_t ou8_InterfaceNumber,
                          const stw::opensyde_core::C_OscCanInterfaceId & orc_CanInterfaceId,
                          const uint32_t ou32_UseCaseIndex);

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeCoOverviewTableView(const C_SdNdeCoOverviewTableView &);
   C_SdNdeCoOverviewTableView & operator =(const C_SdNdeCoOverviewTableView &) &;

   void m_InitColumns(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw::opensyde_gui_logic::C_SdNdeCoOverviewTableModel mc_Model;
   stw::opensyde_gui_logic::C_SdNdeCoOverviewDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
