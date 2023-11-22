//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOPDOTABLEVIEW_HPP
#define C_SDNDECOPDOTABLEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include "C_TblViewScroll.hpp"
#include "C_SdNdeCoPdoTableModel.hpp"
#include "C_OscCanInterfaceId.hpp"
#include "C_SdNdeCoPdoTableDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoPdoTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeCoPdoTableView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeCoPdoTableView(void) override;

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void UpdateData(void);
   void SetNodeIndexAndInterfaceId(const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerInterfaceId,
                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceInterfaceId);
   int32_t GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSwitchToBusProtocolMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
   const;

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeCoPdoTableView(const C_SdNdeCoPdoTableView &);
   C_SdNdeCoPdoTableView & operator =(const C_SdNdeCoPdoTableView &) &;

   void m_InitColumns(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw::opensyde_gui_logic::C_SdNdeCoPdoTableModel mc_Model;
   stw::opensyde_gui_logic::C_SdNdeCoPdoTableDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
