//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboards settings

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSETTINGS_H
#define C_SYVDADASHBOARDSETTINGS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaDashboardSettings;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaDashboardSettings :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardSettings(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                     const stw_types::uint32 ou32_ViewIndex);
   ~C_SyvDaDashboardSettings(void);
   void InitStaticNames(void) const;
   void Save(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SyvDaDashboardSettings * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const stw_types::uint32 mu32_ViewIndex;
   static const stw_types::sintn mhsn_Min;
   static const stw_types::sintn mhsn_Max;
   static const stw_types::sintn mhsn_MinDistanceBetween;

   void m_Load(void) const;
   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_OnFastChanged(const stw_types::sintn osn_Value) const;
   void m_OnMediumChanged(const stw_types::sintn osn_Value) const;
   void m_OnSlowChanged(const stw_types::sintn osn_Value) const;
   //Avoid call
   C_SyvDaDashboardSettings(const C_SyvDaDashboardSettings &);
   C_SyvDaDashboardSettings & operator =(const C_SyvDaDashboardSettings &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
