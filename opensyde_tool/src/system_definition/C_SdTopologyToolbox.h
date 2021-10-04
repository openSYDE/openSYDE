//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for toolbox of system definition

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYTOOLBOX_H
#define C_SDTOPOLOGYTOOLBOX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QLabel>
#include <QSpacerItem>
#include "C_SdTopologyListWidget.h"
#include "C_OSCDeviceDefinition.h"
#include "C_OgeLabTopologyToolboxUserNodes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdTopologyToolbox;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdTopologyToolbox :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdTopologyToolbox(QWidget * const opc_Parent = NULL);
   virtual ~C_SdTopologyToolbox();

   // slot
   void SearchChanged(const QString & orc_Text);
   void InitStaticNames(void) const;

protected:
   virtual void enterEvent(QEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdTopologyToolbox(const C_SdTopologyToolbox &);
   C_SdTopologyToolbox & operator =(const C_SdTopologyToolbox &);

   Ui::C_SdTopologyToolbox * mpc_Ui;
   QVector<C_SdTopologyListWidget *> mc_ListWidgets;

   QIcon mc_Icon;
   C_SdTopologyListWidget * mpc_List;
   QSpacerItem * mpc_Spacer;
   stw_opensyde_gui_elements::C_OgeLabTopologyToolboxUserNodes * mpc_Label;

   void m_FillToolboxDynamic(void);
   void m_FillToolboxStatic(void);
   void m_FillToolboxWithDynamicNodes(const stw_opensyde_core::C_OSCDeviceDefinition & orc_Device);
   void m_IconClearAllClicked(void);
   void m_ErrorHandlingUserFeedback(const QStringList & orc_Errors, const stw_types::sintn & orsn_AddDeviceCount,
                                    const stw_types::sintn & orsn_DeviceCount);
   void m_FileBrowseDialog(void);
   void m_LoadUserDeviceDefinitionPaths(const QString & orc_Path, QStringList & orc_UserDeviceDefPaths,
                                        QStringList & orc_Errors, stw_types::sintn & orsn_DeviceCount) const;
   stw_types::sintn m_AddUserNodesToIni (const QStringList & orc_UserDeviceDefPaths, QStringList & orc_Errors);
   void m_AddUserNodesToToolbox(void);

   stw_types::sint32 m_DeleteUserNode(const QPoint & orc_Pos);
   stw_types::sint32 m_ClearAllUserNodes(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
