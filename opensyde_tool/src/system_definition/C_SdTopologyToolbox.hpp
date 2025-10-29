//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for toolbox of system definition

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYTOOLBOX_HPP
#define C_SDTOPOLOGYTOOLBOX_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QLabel>
#include <QSpacerItem>
#include "C_SdTopologyListWidget.hpp"
#include "C_OscDeviceDefinition.hpp"
#include "C_OgeLabTopologyToolboxUserNodes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdTopologyToolbox;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdTopologyToolbox :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdTopologyToolbox(QWidget * const opc_Parent = NULL);
   ~C_SdTopologyToolbox() override;

   // slot
   void SearchChanged(const QString & orc_Text);
   void InitStaticNames(void) const;

protected:
   void enterEvent(QEnterEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdTopologyToolbox(const C_SdTopologyToolbox &);
   C_SdTopologyToolbox & operator =(const C_SdTopologyToolbox &) &;

   Ui::C_SdTopologyToolbox * mpc_Ui;
   QVector<C_SdTopologyListWidget *> mc_ListWidgets;

   QIcon mc_Icon;
   C_SdTopologyListWidget * mpc_List;
   QSpacerItem * mpc_Spacer;
   stw::opensyde_gui_elements::C_OgeLabTopologyToolboxUserNodes * mpc_Label;

   void m_FillToolboxDynamic(void);
   void m_FillToolboxStatic(void);
   void m_FillToolboxWithDynamicNodes(const stw::opensyde_core::C_OscDeviceDefinition & orc_Device);
   void m_IconClearAllClicked(void);
   void m_ErrorHandlingUserFeedback(const QStringList & orc_Errors, const int32_t & ors32_AddDeviceCount,
                                    const int32_t & ors32_DeviceCount);
   void m_FileBrowseDialog(void);
   void m_LoadUserDeviceDefinitionPaths(const QString & orc_Path, QStringList & orc_UserDeviceDefPaths,
                                        QStringList & orc_Errors, int32_t & ors32_DeviceCount) const;
   int32_t m_AddUserNodesToIni (const QStringList & orc_UserDeviceDefPaths, QStringList & orc_Errors);
   void m_AddUserNodesToToolbox(void);

   int32_t m_DeleteUserNode(const QPoint & orc_Pos);
   int32_t m_ClearAllUserNodes(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
