//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for showing the PDO configuration of a specific device on a specific interface of the CANopen Manager
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOPDOTABLEVIEW_H
#define C_SDNDECOPDOTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include "C_TblViewScroll.h"
#include "C_SdNdeCoPdoTableModel.h"
#include "C_OSCCanInterfaceId.h"
#include "C_SdNdeCoPdoTableDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoPdoTableView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeCoPdoTableView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeCoPdoTableView(void);

   void LoadUserSettings(void); // todo remove constness
   void SaveUserSettings(void) const;
   void UpdateData(void);
   void SetNodeIndexAndInterfaceId(const stw_types::uint32 ou32_ManagerNodeIndex,
                                   const stw_types::uint8 ou8_ManagerInterfaceId,
                                   const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceInterfaceId);
   stw_types::sintn GetCountRows(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigSwitchToBusProtocolMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId)
   const;

protected:
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeCoPdoTableView(const C_SdNdeCoPdoTableView &);
   C_SdNdeCoPdoTableView & operator =(const C_SdNdeCoPdoTableView &);

   void m_InitColumns(void);

   QSortFilterProxyModel mc_SortProxyModel;
   stw_opensyde_gui_logic::C_SdNdeCoPdoTableModel mc_Model;
   stw_opensyde_gui_logic::C_SdNdeCoPdoTableDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
