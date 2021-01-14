//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for the toolbox (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYLISTWIDGET_H
#define C_SDTOPOLOGYLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeListWidgetToolTipBase.h"
#include "C_OgeContextMenu.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdTopologyListWidget :
   public stw_opensyde_gui_elements::C_OgeListWidgetToolTipBase
{
   Q_OBJECT

public:
   explicit C_SdTopologyListWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdTopologyListWidget();

   void SetMaximumHeightAdaption(const bool oq_Active);
   void ApplyDarkMode(const bool oq_Active) const;
   void SetGroupName(const QString & orc_Name);
   void UpdateSize(void);

   bool GetMinimumSize(void) const;
   void SetMinimumSize(const bool oq_MinimumSize);

   static const QString hc_GroupName;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDelete(const QPoint & orc_Pos);

protected:
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdTopologyListWidget(const C_SdTopologyListWidget &);
   C_SdTopologyListWidget & operator =(const C_SdTopologyListWidget &);

   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_DeleteTriggered(void);

   bool mq_MinimumSize;
   bool mq_AdaptMaximumHeight;
   QString mc_Name;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_DeleteAction;
   QPoint mc_Position;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
