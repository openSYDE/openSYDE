//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for toolbox of dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDTOOLBOX_H
#define C_SYVDADASHBOARDTOOLBOX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_SdTopologyListWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaDashboardToolbox;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardToolbox :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardToolbox(QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardToolbox();

   void InitStaticNames(void) const;
   void ApplyDarkMode(const bool oq_Active);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void enterEvent(QEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvDaDashboardToolbox(const C_SyvDaDashboardToolbox &);
   C_SyvDaDashboardToolbox & operator =(const C_SyvDaDashboardToolbox &);

   Ui::C_SyvDaDashboardToolbox * mpc_Ui;
   QVector<C_SdTopologyListWidget *> mc_ListWidgets;

   void m_FillToolboxStatic(void);
   void m_AddWidgetItems(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
